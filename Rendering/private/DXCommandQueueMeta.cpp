#include "DXCommandQueueMeta.h"

namespace
{
	rendering::DXCommandQueueMeta m_meta;
}

rendering::DXCommandQueueMeta::DXCommandQueueMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCommandQueueMeta& rendering::DXCommandQueueMeta::GetInstance()
{
	return m_meta;
}
