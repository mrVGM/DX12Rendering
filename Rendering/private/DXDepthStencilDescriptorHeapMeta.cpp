#include "DXDepthStencilDescriptorHeapMeta.h"

#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::DXDepthStencilDescriptorHeapMeta m_meta;
}

rendering::DXDepthStencilDescriptorHeapMeta::DXDepthStencilDescriptorHeapMeta() :
	BaseObjectMeta(&DXDescriptorHeapMeta::GetInstance())
{
}

const rendering::DXDepthStencilDescriptorHeapMeta& rendering::DXDepthStencilDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
