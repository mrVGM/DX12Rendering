#include "DXDeviceMeta.h"

namespace
{
	rendering::DXDeviceMeta m_meta;
}

rendering::DXDeviceMeta::DXDeviceMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXDeviceMeta& rendering::DXDeviceMeta::GetInstance()
{
	return m_meta;
}
