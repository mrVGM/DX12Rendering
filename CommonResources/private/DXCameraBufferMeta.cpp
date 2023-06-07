#include "DXCameraBufferMeta.h"

#include "DXMutableBufferMeta.h"

namespace
{
	rendering::DXCameraBufferMeta m_meta;
}

rendering::DXCameraBufferMeta::DXCameraBufferMeta() :
	BaseObjectMeta(&DXMutableBufferMeta::GetInstance())
{
}

const rendering::DXCameraBufferMeta& rendering::DXCameraBufferMeta::GetInstance()
{
	return m_meta;
}
