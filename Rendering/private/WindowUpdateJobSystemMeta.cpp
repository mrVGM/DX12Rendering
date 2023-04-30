#include "WindowUpdateJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::WindowUpdateJobSystemMeta m_instance;
}

rendering::WindowUpdateJobSystemMeta::WindowUpdateJobSystemMeta() :
	BaseObjectMeta(jobs::JobSystemMeta::GetInstance())
{
}

const rendering::WindowUpdateJobSystemMeta& rendering::WindowUpdateJobSystemMeta::GetInstance()
{
	return m_instance;
}