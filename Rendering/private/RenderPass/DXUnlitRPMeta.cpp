#include "RenderPass/DXUnlitRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXUnlitRPMeta m_meta;
}

rendering::DXUnlitRPMeta::DXUnlitRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXUnlitRPMeta& rendering::DXUnlitRPMeta::GetInstance()
{
	return m_meta;
}
