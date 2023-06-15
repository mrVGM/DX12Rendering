#include "DXShadowMaskMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::psm::DXShadowMaskMeta m_meta;
}

rendering::psm::DXShadowMaskMeta::DXShadowMaskMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::psm::DXShadowMaskMeta& rendering::psm::DXShadowMaskMeta::GetInstance()
{
	return m_meta;
}
