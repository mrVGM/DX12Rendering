#include "DXSMSettingsBufferMeta.h"

#include "DXMutableBufferMeta.h"

namespace
{
	rendering::psm::DXSMSettingsBufferMeta m_meta;
}

rendering::psm::DXSMSettingsBufferMeta::DXSMSettingsBufferMeta() :
	BaseObjectMeta(&DXMutableBufferMeta::GetInstance())
{
}

const rendering::psm::DXSMSettingsBufferMeta& rendering::psm::DXSMSettingsBufferMeta::GetInstance()
{
	return m_meta;
}
