#include "DXCameraUpdaterMeta.h"

#include "TickUpdaterMeta.h"

namespace
{
	rendering::DXCameraUpdaterMeta m_meta;
}

rendering::DXCameraUpdaterMeta::DXCameraUpdaterMeta() :
	BaseObjectMeta(&TickUpdaterMeta::GetInstance())
{
}

const rendering::DXCameraUpdaterMeta& rendering::DXCameraUpdaterMeta::GetInstance()
{
	return m_meta;
}
