#include "DXGBufferNormalTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferNormalTexMeta m_meta;
}

rendering::deferred::DXGBufferNormalTexMeta::DXGBufferNormalTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferNormalTexMeta& rendering::deferred::DXGBufferNormalTexMeta::GetInstance()
{
	return m_meta;
}
