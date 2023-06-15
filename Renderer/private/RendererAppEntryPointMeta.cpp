#include "RendererAppEntryPointMeta.h"

#include "AppEntryPointMeta.h"
#include "RendererEntryPointMetaTag.h"

namespace
{
	rendering::RendererAppEntryPointMeta m_meta;
}

rendering::RendererAppEntryPointMeta::RendererAppEntryPointMeta() :
	BaseObjectMeta(&settings::AppEntryPointMeta::GetInstance())
{
	m_metaTags.insert(&settings::RendererEntryPointMetaTag::GetInstance());
}

const rendering::RendererAppEntryPointMeta& rendering::RendererAppEntryPointMeta::GetInstance()
{
	return m_meta;
}
