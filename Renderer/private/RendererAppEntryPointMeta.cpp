#include "RendererAppEntryPointMeta.h"

#include "AppEntryPointMeta.h"

namespace
{
	rendering::RendererAppEntryPointMeta m_meta;
}

rendering::RendererAppEntryPointMeta::RendererAppEntryPointMeta() :
	BaseObjectMeta(&settings::AppEntryPointMeta::GetInstance())
{
}

const rendering::RendererAppEntryPointMeta& rendering::RendererAppEntryPointMeta::GetInstance()
{
	return m_meta;
}
