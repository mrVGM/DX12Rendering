#include "Lights/DXShadowMapDSMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXShadowMapDSMeta m_instance;
}

rendering::DXShadowMapDSMeta::DXShadowMapDSMeta() :
	BaseObjectMeta(&rendering::DXTextureMeta::GetInstance())
{
}

const rendering::DXShadowMapDSMeta& rendering::DXShadowMapDSMeta::GetInstance()
{
	return m_instance;
}