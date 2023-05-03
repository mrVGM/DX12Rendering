#include "MainJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::MainJobSystemMeta m_meta;
}

rendering::MainJobSystemMeta::MainJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const rendering::MainJobSystemMeta& rendering::MainJobSystemMeta::GetInstance()
{
	return m_meta;
}
