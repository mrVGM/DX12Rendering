#include "Materials/DXUnlitErrorMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXUnlitErrorMaterialMeta m_meta;
}

rendering::DXUnlitErrorMaterialMeta::DXUnlitErrorMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXUnlitErrorMaterialMeta& rendering::DXUnlitErrorMaterialMeta::GetInstance()
{
	return m_meta;
}
