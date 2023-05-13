#include "DXShadowMapDSDescriptorHeapMeta.h"

#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::DXShadowMapDSDescriptorHeapMeta m_meta;
}

rendering::DXShadowMapDSDescriptorHeapMeta::DXShadowMapDSDescriptorHeapMeta() :
	BaseObjectMeta(&DXDescriptorHeapMeta::GetInstance())
{
}

const rendering::DXShadowMapDSDescriptorHeapMeta& rendering::DXShadowMapDSDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
