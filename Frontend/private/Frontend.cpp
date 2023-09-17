#include "Frontend.h"

#include "JobSystem.h"
#include "Job.h"
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


	return;
}