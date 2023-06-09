#include "LoadJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::LoadJobSystemMeta m_instance;
}

rendering::LoadJobSystemMeta::LoadJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const rendering::LoadJobSystemMeta& rendering::LoadJobSystemMeta::GetInstance()
{
	return m_instance;
}