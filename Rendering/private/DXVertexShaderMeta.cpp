#include "DXVertexShaderMeta.h"

#include "DXShaderMeta.h"

namespace
{
	rendering::DXVertexShaderMeta m_meta;
}

rendering::DXVertexShaderMeta::DXVertexShaderMeta() :
	BaseObjectMeta(&DXShaderMeta::GetInstance())
{
}

const rendering::DXVertexShaderMeta& rendering::DXVertexShaderMeta::GetInstance()
{
	return m_meta;
}
