#include "utils.h"

#include "CombinatorySettingsMeta.h"

#include "ProcessorsJobSystemMeta.h"
#include "ResultJobSystemMeta.h"

#include "JobSystem.h"

#include "BaseObjectContainer.h"

namespace
{
	jobs::JobSystem* m_processorsJobSystem = nullptr;
	jobs::JobSystem* m_resultJobSystem = nullptr;

	void CacheJobSystems()
	{
		using namespace combinatory;

		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		if (!m_processorsJobSystem)
		{
			BaseObject* obj = container.GetObjectOfClass(ProcessorsJobSystemMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Processors Job System!";
			}

			jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
			m_processorsJobSystem = jobSystem;
		}

		if (!m_resultJobSystem)
		{
			BaseObject* obj = container.GetObjectOfClass(ResultJobSystemMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Result Job System!";
			}

			jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
			m_resultJobSystem = jobSystem;
		}
	}
}

combinatory::CombinatorySettings* combinatory::GetSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(CombinatorySettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Combinatory Settings!";
	}

	CombinatorySettings* settings = static_cast<CombinatorySettings*>(obj);
	return settings;
}

int combinatory::GCD(int a, int b)
{
	while (b != 0)
	{
		int t = b;
		b = a % b;
		a = t;
	}

	return a;
}

int combinatory::LCM(int a, int b)
{
	return a * b / GCD(a, b);
}

void combinatory::RunAsync(jobs::Job* job)
{
	CacheJobSystems();

	m_processorsJobSystem->ScheduleJob(job);
}

void combinatory::RunSync(jobs::Job* job)
{
	CacheJobSystems();

	m_resultJobSystem->ScheduleJob(job);
}
