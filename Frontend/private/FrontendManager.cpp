#include "FrontendManager.h"

#include "FrontendManagerMeta.h"

#include "Job.h"

#include "utils.h"

#include "DataLib.h"

#include <sstream>

#include <Windows.h>
#include <string>

frontend::FrontendManager::FrontendManager() :
	BaseObject(frontend::FrontendManagerMeta::GetInstance())
{
	OpenConnection();
}

frontend::FrontendManager::~FrontendManager()
{
	if (m_pipe)
	{
		CloseHandle(m_pipe);
	}

	if (m_frontendProccess > 0)
	{
		std::stringstream ss;
		ss << "taskkill /PID " << m_frontendProccess << " /F";
		system(ss.str().c_str());
	}
}

void frontend::FrontendManager::OpenConnection()
{
	// Try to open a named pipe; wait for it, if necessary. 
	m_pipe = CreateNamedPipe(
		TEXT("\\\\.\\pipe\\mynamedpipe"),   // pipe name 
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		512,
		512,
		0,
		NULL);

	// Break if the pipe handle is valid. 
	if (m_pipe == INVALID_HANDLE_VALUE)
	{
		return;
	}

	std::string frontendPath = data::GetLibrary().GetRootDir() + "..\\JS\\Frontend\\";
	std::string electron = "node_modules\\electron\\dist\\electron.exe";
	std::string cmd = "\"" + frontendPath + electron + "\"" + " " + frontendPath;
	std::wstring wCmd(cmd.begin(), cmd.end());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		const_cast<wchar_t*>(wCmd.c_str()),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	m_frontendProccess = pi.dwProcessId;

	struct Context
	{
		FrontendManager* m_manager = nullptr;
		HANDLE m_hPipe = nullptr;
	};

	Context ctx{ this, m_pipe };

	class Receive : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		Receive(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			const int BUFSIZE = 512;
			TCHAR  chBuf[BUFSIZE];
			BOOL   fSuccess = FALSE;

			DWORD  cbRead, cbToWrite, cbWritten, dwMode;

			while (true)
			{
				// Read from the pipe. 

				fSuccess = ReadFile(
					m_ctx.m_hPipe,    // pipe handle 
					chBuf,    // buffer to receive reply 
					BUFSIZE * sizeof(TCHAR),  // size of buffer 
					&cbRead,  // number of bytes read 
					NULL);    // not overlapped 

				char* data = reinterpret_cast<char*>(chBuf);
				data[cbRead] = 0;

				if (cbRead > 0) {
					std::string command(data);
					if (command == ":quit")
					{
						break;
					}

					const char* message = ":ping";
					if (m_ctx.m_manager->m_toShutdown)
					{
						message = ":quit";
					}

					DWORD written;
					WriteFile(
						m_ctx.m_hPipe,
						message,
						strlen(message),
						&written,
						nullptr);
				}
			}
		}
	};

	RunJob(new Receive(ctx));
}

void frontend::FrontendManager::Shutdown()
{
	m_toShutdown = true;
}
