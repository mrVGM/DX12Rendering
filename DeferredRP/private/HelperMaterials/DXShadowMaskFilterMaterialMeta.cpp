#include "DXShadowMaskFilterMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXShadowMaskFilterMaterialMeta m_meta;
}

rendering::DXShadowMaskFilterMaterialMeta::DXShadowMaskFilterMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXShadowMaskFilterMaterialMeta& rendering::DXShadowMaskFilterMaterialMeta::GetInstance()
{
	return m_meta;
}
