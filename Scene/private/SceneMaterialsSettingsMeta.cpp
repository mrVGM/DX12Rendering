#include "SceneMaterialsSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::SceneMaterialsSettingsMeta m_meta;
}

rendering::SceneMaterialsSettingsMeta::SceneMaterialsSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::SceneMaterialsSettingsMeta& rendering::SceneMaterialsSettingsMeta::GetInstance()
{
	return m_meta;
}
