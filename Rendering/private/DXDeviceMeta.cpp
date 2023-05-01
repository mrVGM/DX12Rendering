#include "DXDeviceMeta.h"

namespace
{
	rendering::DXDeviceMeta m_windowMeta;
}

rendering::DXDeviceMeta::DXDeviceMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXDeviceMeta& rendering::DXDeviceMeta::GetInstance()
{
	return m_windowMeta;
}
