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
	if (!m_mainJobSystem)
	{
		throw "Jobs Library not Initialized!";
	}

	return m_mainJobSystem;
}

