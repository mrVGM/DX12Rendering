#include "DXShadowMapMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::psm::DXShadowMapMaterialMeta m_meta;
}

rendering::psm::DXShadowMapMaterialMeta::DXShadowMapMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::psm::DXShadowMapMaterialMeta& rendering::psm::DXShadowMapMaterialMeta::GetInstance()
{
	return m_meta;
}
