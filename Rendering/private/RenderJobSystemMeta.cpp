#include "RenderJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::RenderJobSystemMeta m_instance;
}

rendering::RenderJobSystemMeta::RenderJobSystemMeta() :
	BaseObjectMeta(jobs::JobSystemMeta::GetInstance())
{
}

const rendering::RenderJobSystemMeta& rendering::RenderJobSystemMeta::GetInstance()
{
	return m_instance;
}