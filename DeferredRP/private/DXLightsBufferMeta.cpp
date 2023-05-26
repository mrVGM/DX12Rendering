#include "DXLightsBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::DXLightsBufferMeta m_meta;
}

rendering::DXLightsBufferMeta::DXLightsBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::DXLightsBufferMeta& rendering::DXLightsBufferMeta::GetInstance()
{
	return m_meta;
}
