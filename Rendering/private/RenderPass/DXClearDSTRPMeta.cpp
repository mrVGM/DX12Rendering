#include "RenderPass/DXClearDSTRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXClearDSTRPMeta m_meta;
}

rendering::DXClearDSTRPMeta::DXClearDSTRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXClearDSTRPMeta& rendering::DXClearDSTRPMeta::GetInstance()
{
	return m_meta;
}
