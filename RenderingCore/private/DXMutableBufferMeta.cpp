#include "DXMutableBufferMeta.h"

namespace
{
	rendering::DXMutableBufferMeta m_meta;
}

rendering::DXMutableBufferMeta::DXMutableBufferMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXMutableBufferMeta& rendering::DXMutableBufferMeta::GetInstance()
{
	return m_meta;
}
