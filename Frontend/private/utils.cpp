#include "utils.h"

#include "BaseObjectContainer.h"

#include "FrontendJobSystemMeta.h"
#include "MainJobSystemMeta.h"

namespace
{
	jobs::JobSystem* m_frontendJobSystem = nullptr;
	jobs::JobSystem* m_mainJobSystem = nullptr;
}

jobs::JobSystem* frontend::GetFrontendJobSystem()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(frontend::FrontendJobSystemMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Frontend Job System!";
	}

	jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
	return jobSystem;
}

jobs::JobSystem* frontend::GetMainJobSystem()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(rendering::MainJobSystemMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Main Job System!";
	}

	jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
	return jobSystem;
}

void frontend::CacheJobSystems()
{
	if (!m_mainJobSystem)
	{
		m_mainJobSystem = GetMainJobSystem();
	}

	if (!m_frontendJobSystem)
	{
		m_frontendJobSystem = GetFrontendJobSystem();
	}
}

void frontend::RunJob(jobs::Job* job)
{
	m_frontendJobSystem->ScheduleJob(job);
}

void frontend::RunSync(jobs::Job* job)
{
	m_mainJobSystem->ScheduleJob(job);
}
