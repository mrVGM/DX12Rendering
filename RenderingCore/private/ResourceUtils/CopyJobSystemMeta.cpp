#include "ResourceUtils/CopyJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::CopyJobSystemMeta m_meta;
}

rendering::CopyJobSystemMeta::CopyJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const rendering::CopyJobSystemMeta& rendering::CopyJobSystemMeta::GetInstance()
{
	return m_meta;
}
