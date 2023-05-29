#include "DXDisplaySMMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXDisplaySMMaterialMeta m_meta;
}

rendering::DXDisplaySMMaterialMeta::DXDisplaySMMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXDisplaySMMaterialMeta& rendering::DXDisplaySMMaterialMeta::GetInstance()
{
	return m_meta;
}
