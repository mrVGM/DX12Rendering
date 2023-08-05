#include "DXOverlayUpdaterMeta.h"

#include "TickUpdaterMeta.h"

namespace
{
	rendering::overlay::DXOverlayUpdaterMeta m_meta;
}

rendering::overlay::DXOverlayUpdaterMeta::DXOverlayUpdaterMeta() :
	BaseObjectMeta(&TickUpdaterMeta::GetInstance())
{
}

const rendering::overlay::DXOverlayUpdaterMeta& rendering::overlay::DXOverlayUpdaterMeta::GetInstance()
{
	return m_meta;
}
