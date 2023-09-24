#include "utils.h"

#include "BaseObjectContainer.h"

#include "Jobs.h"

#include "FrontendJobSystemMeta.h"
#include "MainJobSystemMeta.h"

#include "FrontendManagerMeta.h"

namespace
{
	jobs::JobSystem* m_frontendJobSystem = nullptr;
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

	BaseObject* obj = container.GetObjectOfClass(jobs::MainJobSystemMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Main Job System!";
	}

	jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
	return jobSystem;
}

frontend::FrontendManager* frontend::GetFrontendManger()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(frontend::FrontendManagerMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Frontend Manager!";
	}

	frontend::FrontendManager* frontendManager = static_cast<frontend::FrontendManager*>(obj);
	return frontendManager;
}

void frontend::CacheJobSystems()
{
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
	jobs::GetMainJobSystem()->ScheduleJob(job);
}
