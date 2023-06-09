#include "DXShadowMaskMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXShadowMaskMaterialMeta m_meta;
}

rendering::DXShadowMaskMaterialMeta::DXShadowMaskMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXShadowMaskMaterialMeta& rendering::DXShadowMaskMaterialMeta::GetInstance()
{
	return m_meta;
}
