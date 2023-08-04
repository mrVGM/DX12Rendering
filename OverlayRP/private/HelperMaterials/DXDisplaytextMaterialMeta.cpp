#include "DXDisplayTextMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::overlay::DXDisplayTextMaterialMeta m_meta;
}

rendering::overlay::DXDisplayTextMaterialMeta::DXDisplayTextMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::overlay::DXDisplayTextMaterialMeta& rendering::overlay::DXDisplayTextMaterialMeta::GetInstance()
{
	return m_meta;
}
