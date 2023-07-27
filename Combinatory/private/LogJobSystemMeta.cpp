#include "LogJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	combinatory::LogJobSystemMeta m_meta;
}

combinatory::LogJobSystemMeta::LogJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const combinatory::LogJobSystemMeta& combinatory::LogJobSystemMeta::GetInstance()
{
	return m_meta;
}
