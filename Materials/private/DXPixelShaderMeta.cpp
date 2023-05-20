#include "DXPixelShaderMeta.h"

#include "DXShaderMeta.h"

namespace
{
	rendering::DXPixelShaderMeta m_meta;
}

rendering::DXPixelShaderMeta::DXPixelShaderMeta() :
	BaseObjectMeta(&DXShaderMeta::GetInstance())
{
}

const rendering::DXPixelShaderMeta& rendering::DXPixelShaderMeta::GetInstance()
{
	return m_meta;
}
