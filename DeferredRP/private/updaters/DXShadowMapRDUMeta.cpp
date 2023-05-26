#include "DXShadowMapRDUMeta.h"

#include "RenderDataUpdaterMeta.h"

namespace
{
	rendering::DXShadowMapRDUMeta m_meta;
}

rendering::DXShadowMapRDUMeta::DXShadowMapRDUMeta() :
	BaseObjectMeta(&RenderDataUpdaterMeta::GetInstance())
{
}

const rendering::DXShadowMapRDUMeta& rendering::DXShadowMapRDUMeta::GetInstance()
{
	return m_meta;
}
