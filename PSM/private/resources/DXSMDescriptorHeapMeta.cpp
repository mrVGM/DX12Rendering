#include "DXSMDescriptorHeapMeta.h"

#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::psm::DXSMDescriptorHeapMeta m_meta;
}

rendering::psm::DXSMDescriptorHeapMeta::DXSMDescriptorHeapMeta() :
	BaseObjectMeta(&DXDescriptorHeapMeta::GetInstance())
{
}

const rendering::psm::DXSMDescriptorHeapMeta& rendering::psm::DXSMDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
