#include "OutlineSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::OutlineSettingsMeta m_meta;
}

rendering::OutlineSettingsMeta::OutlineSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::OutlineSettingsMeta& rendering::OutlineSettingsMeta::GetInstance()
{
	return m_meta;
}
