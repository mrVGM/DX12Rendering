#include "Deferred/DXGBufferAmbientLitTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferAmbientLitTexMeta m_meta;
}

rendering::deferred::DXGBufferAmbientLitTexMeta::DXGBufferAmbientLitTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferAmbientLitTexMeta& rendering::deferred::DXGBufferAmbientLitTexMeta::GetInstance()
{
	return m_meta;
}
