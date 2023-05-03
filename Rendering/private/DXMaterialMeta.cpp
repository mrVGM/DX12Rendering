#include "DXMaterialMeta.h"

namespace
{
	rendering::DXMaterialMeta m_meta;
}

rendering::DXMaterialMeta::DXMaterialMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXMaterialMeta& rendering::DXMaterialMeta::GetInstance()
{
	return m_meta;
}
