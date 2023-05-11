#include "Deferred/DXGBufferSpecularLitTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferSpecularLitTexMeta m_meta;
}

rendering::deferred::DXGBufferSpecularLitTexMeta::DXGBufferSpecularLitTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferSpecularLitTexMeta& rendering::deferred::DXGBufferSpecularLitTexMeta::GetInstance()
{
	return m_meta;
}
