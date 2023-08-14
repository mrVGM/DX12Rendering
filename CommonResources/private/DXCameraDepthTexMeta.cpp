#include "DXCameraDepthTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXCameraDepthTexMeta m_meta;
}

rendering::DXCameraDepthTexMeta::DXCameraDepthTexMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::DXCameraDepthTexMeta& rendering::DXCameraDepthTexMeta::GetInstance()
{
	return m_meta;
}
