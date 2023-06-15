#include "DXShadowMapMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::psm::DXShadowMapMeta m_meta;
}

rendering::psm::DXShadowMapMeta::DXShadowMapMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::psm::DXShadowMapMeta& rendering::psm::DXShadowMapMeta::GetInstance()
{
	return m_meta;
}
