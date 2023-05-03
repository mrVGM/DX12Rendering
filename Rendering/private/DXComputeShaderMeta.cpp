#include "DXComputeShaderMeta.h"

#include "DXShaderMeta.h"

namespace
{
	rendering::DXComputeShaderMeta m_meta;
}

rendering::DXComputeShaderMeta::DXComputeShaderMeta() :
	BaseObjectMeta(&DXShaderMeta::GetInstance())
{
}

const rendering::DXComputeShaderMeta& rendering::DXComputeShaderMeta::GetInstance()
{
	return m_meta;
}
