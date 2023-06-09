#include "DXShadowMapUpdaterMeta.h"

#include "TickUpdaterMeta.h"

namespace
{
	rendering::DXShadowMapUpdaterMeta m_meta;
}

rendering::DXShadowMapUpdaterMeta::DXShadowMapUpdaterMeta() :
	BaseObjectMeta(&TickUpdaterMeta::GetInstance())
{
}

const rendering::DXShadowMapUpdaterMeta& rendering::DXShadowMapUpdaterMeta::GetInstance()
{
	return m_meta;
}
