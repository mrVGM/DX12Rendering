#include "FrontendManager.h"

#include "FrontendManagerMeta.h"

#include "Job.h"

#include "utils.h"

#include "DataLib.h"

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
	std::string startCommand = frontendPath + "run.bat";
	system(startCommand.c_str());

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
					std::string setting(data);
				}
			}

			CloseHandle(m_ctx.m_hPipe);
		}
	};

	RunJob(new Receive(ctx));
}
