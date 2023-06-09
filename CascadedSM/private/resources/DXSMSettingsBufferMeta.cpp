#include "DXSMSettingsBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::DXSMSettingsBufferMeta m_meta;
}

rendering::DXSMSettingsBufferMeta::DXSMSettingsBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::DXSMSettingsBufferMeta& rendering::DXSMSettingsBufferMeta::GetInstance()
{
	return m_meta;
}
