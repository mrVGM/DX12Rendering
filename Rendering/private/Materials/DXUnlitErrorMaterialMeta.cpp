#include "Materials/DXUnlitErrorMaterialMeta.h"
#include "Materials/DXUnlitMaterialMetaTag.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXUnlitErrorMaterialMeta m_meta;
}

rendering::DXUnlitErrorMaterialMeta::DXUnlitErrorMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
	m_metaTags.insert(&DXUnlitMaterialMetaTag::GetInstance());
}

const rendering::DXUnlitErrorMaterialMeta& rendering::DXUnlitErrorMaterialMeta::GetInstance()
{
	return m_meta;
}
