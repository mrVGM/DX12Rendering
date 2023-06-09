#include "DXShadowMapFilterMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXShadowMapFilterMaterialMeta m_meta;
}

rendering::DXShadowMapFilterMaterialMeta::DXShadowMapFilterMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXShadowMapFilterMaterialMeta& rendering::DXShadowMapFilterMaterialMeta::GetInstance()
{
	return m_meta;
}
