#include "Deferred/DXGBufferDuffuseTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::deferred::DXGBufferDuffuseTexMeta m_meta;
}

rendering::deferred::DXGBufferDuffuseTexMeta::DXGBufferDuffuseTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::deferred::DXGBufferDuffuseTexMeta& rendering::deferred::DXGBufferDuffuseTexMeta::GetInstance()
{
	return m_meta;
}
