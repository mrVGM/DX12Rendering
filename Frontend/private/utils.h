#pragma once

#include "JobSystem.h"
#include "Job.h"

namespace frontend
{
	jobs::JobSystem* GetFrontendJobSystem();
	jobs::JobSystem* GetMainJobSystem();
	void CacheJobSystems();

	void RunJob(jobs::Job* job);
	void RunSync(jobs::Job* job);
}