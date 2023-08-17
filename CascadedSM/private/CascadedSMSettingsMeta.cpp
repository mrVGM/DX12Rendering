#include "CascadedSMSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::CascadedSMSettingsMeta m_meta;
}

rendering::CascadedSMSettingsMeta::CascadedSMSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::CascadedSMSettingsMeta& rendering::CascadedSMSettingsMeta::GetInstance()
{
	return m_meta;
}
