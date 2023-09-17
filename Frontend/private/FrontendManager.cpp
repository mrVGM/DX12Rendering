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
	m_pipe = CreateFile(
		TEXT("\\\\.\\pipe\\mynamedpipe"),   // pipe name 
		GENERIC_READ,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,	// opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 


	std::string frontendPath = data::GetLibrary().GetRootDir() + "..\\JS\\Frontend\\";
	std::string startCommand = frontendPath + "run.bat";
	system(startCommand.c_str());

	// Break if the pipe handle is valid. 
	if (m_pipe == INVALID_HANDLE_VALUE)
		return;

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
