#include "DXShadowMapMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXShadowMapMaterialMeta m_meta;
}

rendering::DXShadowMapMaterialMeta::DXShadowMapMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXShadowMapMaterialMeta& rendering::DXShadowMapMaterialMeta::GetInstance()
{
	return m_meta;
}
