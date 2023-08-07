#include "SceneConverterSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	scene_converter::SceneConverterSettingsMeta m_meta;
}

scene_converter::SceneConverterSettingsMeta::SceneConverterSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const scene_converter::SceneConverterSettingsMeta& scene_converter::SceneConverterSettingsMeta::GetInstance()
{
	return m_meta;
}
