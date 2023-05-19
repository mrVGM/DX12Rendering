#include "ResidentHeapJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	rendering::ResidentHeapJobSystemMeta m_meta;
}

rendering::ResidentHeapJobSystemMeta::ResidentHeapJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const rendering::ResidentHeapJobSystemMeta& rendering::ResidentHeapJobSystemMeta::GetInstance()
{
	return m_meta;
}
