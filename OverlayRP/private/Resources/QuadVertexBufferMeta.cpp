#include "QuadVertexBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::overlay::QuadVertexBufferMeta m_meta;
}

rendering::overlay::QuadVertexBufferMeta::QuadVertexBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::overlay::QuadVertexBufferMeta& rendering::overlay::QuadVertexBufferMeta::GetInstance()
{
	return m_meta;
}
