#include "DXFenceMeta.h"

namespace
{
	rendering::DXFenceMeta m_meta;
}

rendering::DXFenceMeta::DXFenceMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXFenceMeta& rendering::DXFenceMeta::GetInstance()
{
	return m_meta;
}
