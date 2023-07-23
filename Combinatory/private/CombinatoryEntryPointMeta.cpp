#include "CombinatoryEntryPointMeta.h"

#include "AppEntryPointMeta.h"

#include "CombinatoryEntryPointMetaTag.h"

namespace
{
	combinatory::CombinatoryEntryPointMeta m_meta;
}

combinatory::CombinatoryEntryPointMeta::CombinatoryEntryPointMeta() :
	BaseObjectMeta(&settings::AppEntryPointMeta::GetInstance())
{
	m_metaTags.insert(&settings::CombinatoryEntryPointMetaTag::GetInstance());
}

const combinatory::CombinatoryEntryPointMeta& combinatory::CombinatoryEntryPointMeta::GetInstance()
{
	return m_meta;
}
