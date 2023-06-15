#include "SceneConverterEntryPointMetaTag.h"

namespace
{
	settings::SceneConverterEntryPointMetaTag m_metaTag;
}

settings::SceneConverterEntryPointMetaTag::SceneConverterEntryPointMetaTag()
{
}

const settings::SceneConverterEntryPointMetaTag& settings::SceneConverterEntryPointMetaTag::GetInstance()
{
	return m_metaTag;
}
