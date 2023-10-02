#include "ReflectionSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	reflection::ReflectionSettingsMeta m_meta;
}

reflection::ReflectionSettingsMeta::ReflectionSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const reflection::ReflectionSettingsMeta& reflection::ReflectionSettingsMeta::GetInstance()
{
	return m_meta;
}
