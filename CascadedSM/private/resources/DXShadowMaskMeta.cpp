#include "DXShadowMaskMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXShadowMaskMeta m_instance;
}

rendering::DXShadowMaskMeta::DXShadowMaskMeta() :
	BaseObjectMeta(&rendering::DXTextureMeta::GetInstance())
{
}

const rendering::DXShadowMaskMeta& rendering::DXShadowMaskMeta::GetInstance()
{
	return m_instance;
}