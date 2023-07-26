#include "ProcessorsJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	combinatory::ProcessorsJobSystemMeta m_meta;
}

combinatory::ProcessorsJobSystemMeta::ProcessorsJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const combinatory::ProcessorsJobSystemMeta& combinatory::ProcessorsJobSystemMeta::GetInstance()
{
	return m_meta;
}
