#include "DXOverlayRPMeta.h"

#include "RenderPassMeta.h"

namespace
{
	rendering::overlay::DXOverlayRPMeta m_meta;
}

rendering::overlay::DXOverlayRPMeta::DXOverlayRPMeta() :
	BaseObjectMeta(&RenderPassMeta::GetInstance())
{
}

const rendering::overlay::DXOverlayRPMeta& rendering::overlay::DXOverlayRPMeta::GetInstance()
{
	return m_meta;
}
