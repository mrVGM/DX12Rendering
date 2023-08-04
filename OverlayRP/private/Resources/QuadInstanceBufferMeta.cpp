#include "QuadInstanceBufferMeta.h"

#include "DXMutableBufferMeta.h"

namespace
{
	rendering::overlay::QuadInstanceBufferMeta m_meta;
}

rendering::overlay::QuadInstanceBufferMeta::QuadInstanceBufferMeta() :
	BaseObjectMeta(&DXMutableBufferMeta::GetInstance())
{
}

const rendering::overlay::QuadInstanceBufferMeta& rendering::overlay::QuadInstanceBufferMeta::GetInstance()
{
	return m_meta;
}
