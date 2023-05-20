#include "DXCameraBufferMeta.h"

#include "DXBufferMeta.h"

namespace
{
	rendering::DXCameraBufferMeta m_meta;
}

rendering::DXCameraBufferMeta::DXCameraBufferMeta() :
	BaseObjectMeta(&DXBufferMeta::GetInstance())
{
}

const rendering::DXCameraBufferMeta& rendering::DXCameraBufferMeta::GetInstance()
{
	return m_meta;
}
