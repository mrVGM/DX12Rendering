#include "Materials/DXDeferredMaterialMetaTag.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXDeferredMaterialMetaTag m_metaTag;
}

rendering::DXDeferredMaterialMetaTag::DXDeferredMaterialMetaTag()
{
}

const rendering::DXDeferredMaterialMetaTag& rendering::DXDeferredMaterialMetaTag::GetInstance()
{
	return m_metaTag;
}
