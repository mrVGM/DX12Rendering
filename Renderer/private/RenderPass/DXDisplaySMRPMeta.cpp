#include "RenderPass/DXDisplaySMRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXDisplaySMRPMeta m_meta;
}

rendering::DXDisplaySMRPMeta::DXDisplaySMRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXDisplaySMRPMeta& rendering::DXDisplaySMRPMeta::GetInstance()
{
	return m_meta;
}
