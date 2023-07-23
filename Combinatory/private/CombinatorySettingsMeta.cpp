#include "CombinatorySettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	combinatory::CombinatorySettingsMeta m_meta;
}

combinatory::CombinatorySettingsMeta::CombinatorySettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const combinatory::CombinatorySettingsMeta& combinatory::CombinatorySettingsMeta::GetInstance()
{
	return m_meta;
}
