#include "AppSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	settings::AppSettingsMeta m_meta;
}

settings::AppSettingsMeta::AppSettingsMeta() :
	BaseObjectMeta(&SettingsReaderMeta::GetInstance())
{
}

const settings::AppSettingsMeta& settings::AppSettingsMeta::GetInstance()
{
	return m_meta;
}
