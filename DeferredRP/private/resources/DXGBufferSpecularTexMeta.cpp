#include "DXGBufferSpecularTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferSpecularTexMeta  m_meta;
}

rendering::deferred::DXGBufferSpecularTexMeta::DXGBufferSpecularTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferSpecularTexMeta& rendering::deferred::DXGBufferSpecularTexMeta::GetInstance()
{
	return m_meta;
}
