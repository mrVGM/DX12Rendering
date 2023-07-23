#include "CombinatoryEntryPointMetaTag.h"

namespace
{
	settings::CombinatoryEntryPointMetaTag m_metaTag;
}

settings::CombinatoryEntryPointMetaTag::CombinatoryEntryPointMetaTag()
{
}

const settings::CombinatoryEntryPointMetaTag& settings::CombinatoryEntryPointMetaTag::GetInstance()
{
	return m_metaTag;
}
