#include "DXCameraBufferMeta.h"

namespace
{
	rendering::DXCameraBufferMeta m_meta;
}

rendering::DXCameraBufferMeta::DXCameraBufferMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCameraBufferMeta& rendering::DXCameraBufferMeta::GetInstance()
{
	return m_meta;
}
