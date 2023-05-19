#include "DXHeapMeta.h"

namespace
{
	rendering::DXHeapMeta m_meta;
}

rendering::DXHeapMeta::DXHeapMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXHeapMeta& rendering::DXHeapMeta::GetInstance()
{
	return m_meta;
}
