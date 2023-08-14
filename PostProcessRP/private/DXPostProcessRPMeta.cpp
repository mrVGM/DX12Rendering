#include "DXPostProcessRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXPostProcessRPMeta m_meta;
}

rendering::DXPostProcessRPMeta::DXPostProcessRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXPostProcessRPMeta& rendering::DXPostProcessRPMeta::GetInstance()
{
	return m_meta;
}
