#include "QuadIndexBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::overlay::QuadIndexBufferMeta m_meta;
}

rendering::overlay::QuadIndexBufferMeta::QuadIndexBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::overlay::QuadIndexBufferMeta& rendering::overlay::QuadIndexBufferMeta::GetInstance()
{
	return m_meta;
}
