#include "RendererEntryPointMetaTag.h"

namespace
{
	settings::RendererEntryPointMetaTag m_metaTag;
}

settings::RendererEntryPointMetaTag::RendererEntryPointMetaTag()
{
}

const settings::RendererEntryPointMetaTag& settings::RendererEntryPointMetaTag::GetInstance()
{
	return m_metaTag;
}
