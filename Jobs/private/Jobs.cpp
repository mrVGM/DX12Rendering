#include "Jobs.h"

#include "JobSystem.h"
#include "MainJobSystemMeta.h"

#include "BaseObjectContainer.h"

namespace
{
	jobs::JobSystem* m_mainJobSystem = nullptr;
}

void jobs::Boot()
{
	if (m_mainJobSystem)
	{
		return;
	}

	m_mainJobSystem = new JobSystem(MainJobSystemMeta::GetInstance(), 1);
}

jobs::JobSystem* jobs::GetMainJobSystem()
{
	return m_mainJobSystem;
}