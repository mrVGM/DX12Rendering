#include "DXShadowMapMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXShadowMapMeta m_instance;
}

rendering::DXShadowMapMeta::DXShadowMapMeta() :
	BaseObjectMeta(&rendering::DXTextureMeta::GetInstance())
{
}

const rendering::DXShadowMapMeta& rendering::DXShadowMapMeta::GetInstance()
{
	return m_instance;
}