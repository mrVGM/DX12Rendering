#include "ResourceUtils/DXCopyBuffersMeta.h"

namespace
{
	rendering::DXCopyBuffersMeta m_meta;
}

rendering::DXCopyBuffersMeta::DXCopyBuffersMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCopyBuffersMeta& rendering::DXCopyBuffersMeta::GetInstance()
{
	return m_meta;
}
