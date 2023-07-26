#include "ResultJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	combinatory::ResultJobSystemMeta m_meta;
}

combinatory::ResultJobSystemMeta::ResultJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const combinatory::ResultJobSystemMeta& combinatory::ResultJobSystemMeta::GetInstance()
{
	return m_meta;
}
