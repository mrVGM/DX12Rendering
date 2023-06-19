#include "DXShadowMaskMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::psm::DXShadowMaskMaterialMeta m_meta;
}

rendering::psm::DXShadowMaskMaterialMeta::DXShadowMaskMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::psm::DXShadowMaskMaterialMeta& rendering::psm::DXShadowMaskMaterialMeta::GetInstance()
{
	return m_meta;
}
