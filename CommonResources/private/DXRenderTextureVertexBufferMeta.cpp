#include "DXRenderTextureVertexBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::DXRenderTextureVertexBufferMeta m_meta;
}

rendering::DXRenderTextureVertexBufferMeta::DXRenderTextureVertexBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::DXRenderTextureVertexBufferMeta& rendering::DXRenderTextureVertexBufferMeta::GetInstance()
{
	return m_meta;
}
