#include "DXDeferredRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::DXDeferredRPMeta m_meta;
}

rendering::DXDeferredRPMeta::DXDeferredRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::DXDeferredRPMeta& rendering::DXDeferredRPMeta::GetInstance()
{
	return m_meta;
}
