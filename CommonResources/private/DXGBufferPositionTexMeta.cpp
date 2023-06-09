#include "DXGBufferPositionTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferPositionTexMeta m_meta;
}

rendering::deferred::DXGBufferPositionTexMeta::DXGBufferPositionTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferPositionTexMeta& rendering::deferred::DXGBufferPositionTexMeta::GetInstance()
{
	return m_meta;
}
