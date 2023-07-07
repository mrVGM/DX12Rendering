#include "ShaderRepoSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::ShaderRepoSettingsMeta m_meta;
}

rendering::ShaderRepoSettingsMeta::ShaderRepoSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::ShaderRepoSettingsMeta& rendering::ShaderRepoSettingsMeta::GetInstance()
{
	return m_meta;
}
