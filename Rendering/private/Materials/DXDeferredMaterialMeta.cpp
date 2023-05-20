#include "DXDeferredMaterialMeta.h"
#include "DXDeferredMaterialMetaTag.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXDeferredMaterialMeta m_meta;
}

rendering::DXDeferredMaterialMeta::DXDeferredMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
	m_metaTags.insert(&DXDeferredMaterialMetaTag::GetInstance());
}

const rendering::DXDeferredMaterialMeta& rendering::DXDeferredMaterialMeta::GetInstance()
{
	return m_meta;
}
