#include "DXCopyCommandQueueMeta.h"

namespace
{
	rendering::DXCopyCommandQueueMeta m_meta;
}

rendering::DXCopyCommandQueueMeta::DXCopyCommandQueueMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCopyCommandQueueMeta& rendering::DXCopyCommandQueueMeta::GetInstance()
{
	return m_meta;
}
