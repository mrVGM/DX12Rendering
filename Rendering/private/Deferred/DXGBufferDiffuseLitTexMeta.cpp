#include "Deferred/DXGBufferDiffuseLitTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferDiffuseLitTexMeta m_meta;
}

rendering::deferred::DXGBufferDiffuseLitTexMeta::DXGBufferDiffuseLitTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferDiffuseLitTexMeta& rendering::deferred::DXGBufferDiffuseLitTexMeta::GetInstance()
{
	return m_meta;
}
