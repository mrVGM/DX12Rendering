#include "ReflectionEntryPointMetaTag.h"

namespace
{
	settings::ReflectionEntryPointMetaTag m_metaTag;
}

settings::ReflectionEntryPointMetaTag::ReflectionEntryPointMetaTag()
{
}

const settings::ReflectionEntryPointMetaTag& settings::ReflectionEntryPointMetaTag::GetInstance()
{
	return m_metaTag;
}
