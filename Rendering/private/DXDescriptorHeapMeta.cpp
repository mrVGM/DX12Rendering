#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::DXDescriptorHeapMeta m_meta;
}

rendering::DXDescriptorHeapMeta::DXDescriptorHeapMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXDescriptorHeapMeta& rendering::DXDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
