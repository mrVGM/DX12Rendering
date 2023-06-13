#include "DXSMSettingsBufferMeta.h"

#include "DXMutableBufferMeta.h"

namespace
{
	rendering::DXSMSettingsBufferMeta m_meta;
}

rendering::DXSMSettingsBufferMeta::DXSMSettingsBufferMeta() :
	BaseObjectMeta(&DXMutableBufferMeta::GetInstance())
{
}

const rendering::DXSMSettingsBufferMeta& rendering::DXSMSettingsBufferMeta::GetInstance()
{
	return m_meta;
}
