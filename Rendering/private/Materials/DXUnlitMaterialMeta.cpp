#include "Materials/DXUnlitMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXUnlitMaterialMeta m_meta;
}

rendering::DXUnlitMaterialMeta::DXUnlitMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXUnlitMaterialMeta& rendering::DXUnlitMaterialMeta::GetInstance()
{
	return m_meta;
}
