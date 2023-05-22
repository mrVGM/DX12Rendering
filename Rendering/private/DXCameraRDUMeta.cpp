#include "DXCameraRDUMeta.h"

#include "RenderDataUpdaterMeta.h"

namespace
{
	rendering::DXCameraRDUMeta m_meta;
}

rendering::DXCameraRDUMeta::DXCameraRDUMeta() :
	BaseObjectMeta(&RenderDataUpdaterMeta::GetInstance())
{
}

const rendering::DXCameraRDUMeta& rendering::DXCameraRDUMeta::GetInstance()
{
	return m_meta;
}
