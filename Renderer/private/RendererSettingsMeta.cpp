#include "RendererSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::RendererSettingsMeta m_meta;
}

rendering::RendererSettingsMeta::RendererSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::RendererSettingsMeta& rendering::RendererSettingsMeta::GetInstance()
{
	return m_meta;
}
