#include "SceneSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	collada::SceneSettingsMeta m_meta;
}

collada::SceneSettingsMeta::SceneSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const collada::SceneSettingsMeta& collada::SceneSettingsMeta::GetInstance()
{
	return m_meta;
}
