#include "CanvasVertexBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::CanvasVertexBufferMeta m_meta;
}

rendering::CanvasVertexBufferMeta::CanvasVertexBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::CanvasVertexBufferMeta& rendering::CanvasVertexBufferMeta::GetInstance()
{
	return m_meta;
}
