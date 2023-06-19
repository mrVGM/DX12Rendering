#include "DXSMDSDescriptorHeapMeta.h"

#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::psm::DXSMDSDescriptorHeapMeta m_meta;
}

rendering::psm::DXSMDSDescriptorHeapMeta::DXSMDSDescriptorHeapMeta() :
	BaseObjectMeta(&DXDescriptorHeapMeta::GetInstance())
{
}

const rendering::psm::DXSMDSDescriptorHeapMeta& rendering::psm::DXSMDSDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
