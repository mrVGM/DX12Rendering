#include "DXShadowMapFilterTexMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXShadowMapFilterTexMeta m_instance;
}

rendering::DXShadowMapFilterTexMeta::DXShadowMapFilterTexMeta() :
	BaseObjectMeta(&rendering::DXTextureMeta::GetInstance())
{
}

const rendering::DXShadowMapFilterTexMeta& rendering::DXShadowMapFilterTexMeta::GetInstance()
{
	return m_instance;
}