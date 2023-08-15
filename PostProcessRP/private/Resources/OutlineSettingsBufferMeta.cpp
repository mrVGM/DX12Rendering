#include "OutlineSettingsBufferMeta.h"

#include "DXMutableBufferMeta.h"

namespace
{
	rendering::OutlineSettingsBufferMeta m_meta;
}

rendering::OutlineSettingsBufferMeta::OutlineSettingsBufferMeta() :
	BaseObjectMeta(&DXMutableBufferMeta::GetInstance())
{
}

const rendering::OutlineSettingsBufferMeta& rendering::OutlineSettingsBufferMeta::GetInstance()
{
	return m_meta;
}
