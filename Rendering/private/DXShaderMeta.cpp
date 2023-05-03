#include "DXShaderMeta.h"

namespace
{
	rendering::DXShaderMeta m_meta;
}

rendering::DXShaderMeta::DXShaderMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXShaderMeta& rendering::DXShaderMeta::GetInstance()
{
	return m_meta;
}
