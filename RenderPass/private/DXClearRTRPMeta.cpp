#include "DXClearRTRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXClearRTRPMeta m_meta;
}

rendering::DXClearRTRPMeta::DXClearRTRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXClearRTRPMeta& rendering::DXClearRTRPMeta::GetInstance()
{
	return m_meta;
}
