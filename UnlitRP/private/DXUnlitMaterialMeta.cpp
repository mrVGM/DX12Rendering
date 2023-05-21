#include "DXUnlitMaterialMeta.h"
#include "DXUnlitMaterialMetaTag.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXUnlitMaterialMeta m_meta;
}

rendering::DXUnlitMaterialMeta::DXUnlitMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
	m_metaTags.insert(&DXUnlitMaterialMetaTag::GetInstance());
}

const rendering::DXUnlitMaterialMeta& rendering::DXUnlitMaterialMeta::GetInstance()
{
	return m_meta;
}
