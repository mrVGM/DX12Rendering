#include "Frontend.h"

#include "JobSystem.h"
#include "Job.h"
#include "Jobs.h"
#include "FrontendJobSystemMeta.h"

#include "FrontendManager.h"

#include "utils.h"

void frontend::Boot()
{
	class Init : public jobs::Job
	{
	public:
		void Do() override
		{
			new jobs::JobSystem(frontend::FrontendJobSystemMeta::GetInstance(), 1);
			CacheJobSystems();
			new FrontendManager();
		}
	};

	RunSync(new Init());

	return;
}

void frontend::Shutdown()
{
	class Shutdown : public jobs::Job
	{
	public:
		void Do() override
		{
			FrontendManager* frontendManager = GetFrontendManager();
			frontendManager->Shutdown();
		}
	};

	RunSync(new Shutdown());

	return;
}