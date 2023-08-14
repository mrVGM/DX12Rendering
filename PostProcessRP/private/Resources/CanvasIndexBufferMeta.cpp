#include "CanvasIndexBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::CanvasIndexBufferMeta m_meta;
}

rendering::CanvasIndexBufferMeta::CanvasIndexBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::CanvasIndexBufferMeta& rendering::CanvasIndexBufferMeta::GetInstance()
{
	return m_meta;
}
