#pragma once

#include "JobSystem.h"
#include "Job.h"
#include "FrontendManager.h"

namespace frontend
{
	jobs::JobSystem* GetFrontendJobSystem();
	jobs::JobSystem* GetMainJobSystem();
	FrontendManager* GetFrontendManger();
	void CacheJobSystems();

	void RunJob(jobs::Job* job);
	void RunSync(jobs::Job* job);
}