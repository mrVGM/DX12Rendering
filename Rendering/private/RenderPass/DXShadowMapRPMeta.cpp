#include "RenderPass/DXShadowMapRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXShadowMapRPMeta m_meta;
}

rendering::DXShadowMapRPMeta::DXShadowMapRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXShadowMapRPMeta& rendering::DXShadowMapRPMeta::GetInstance()
{
	return m_meta;
}
