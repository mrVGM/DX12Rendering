#include "DXBufferMeta.h"

namespace
{
	rendering::DXBufferMeta m_meta;
}

rendering::DXBufferMeta::DXBufferMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXBufferMeta& rendering::DXBufferMeta::GetInstance()
{
	return m_meta;
}
