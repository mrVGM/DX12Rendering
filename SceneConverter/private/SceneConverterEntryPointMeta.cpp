#include "SceneConverterEntryPointMeta.h"

#include "AppEntryPointMeta.h"

namespace
{
	scene_converter::SceneConverterEntryPointMeta m_meta;
}

scene_converter::SceneConverterEntryPointMeta::SceneConverterEntryPointMeta() :
	BaseObjectMeta(&settings::AppEntryPointMeta::GetInstance())
{
}

const scene_converter::SceneConverterEntryPointMeta& scene_converter::SceneConverterEntryPointMeta::GetInstance()
{
	return m_meta;
}
