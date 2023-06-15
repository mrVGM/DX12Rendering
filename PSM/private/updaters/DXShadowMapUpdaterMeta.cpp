#include "DXShadowMapUpdaterMeta.h"

#include "TickUpdaterMeta.h"

namespace
{
	rendering::psm::DXShadowMapUpdaterMeta m_meta;
}

rendering::psm::DXShadowMapUpdaterMeta::DXShadowMapUpdaterMeta() :
	BaseObjectMeta(&TickUpdaterMeta::GetInstance())
{
}

const rendering::psm::DXShadowMapUpdaterMeta& rendering::psm::DXShadowMapUpdaterMeta::GetInstance()
{
	return m_meta;
}
