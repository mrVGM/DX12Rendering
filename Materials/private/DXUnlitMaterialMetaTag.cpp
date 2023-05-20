#include "DXUnlitMaterialMetaTag.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXUnlitMaterialMetaTag m_metaTag;
}

rendering::DXUnlitMaterialMetaTag::DXUnlitMaterialMetaTag()
{
}

const rendering::DXUnlitMaterialMetaTag& rendering::DXUnlitMaterialMetaTag::GetInstance()
{
	return m_metaTag;
}
