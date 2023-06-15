#include "DXShadowMapDSMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::psm::DXShadowMapDSMeta m_meta;
}

rendering::psm::DXShadowMapDSMeta::DXShadowMapDSMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::psm::DXShadowMapDSMeta& rendering::psm::DXShadowMapDSMeta::GetInstance()
{
	return m_meta;
}
