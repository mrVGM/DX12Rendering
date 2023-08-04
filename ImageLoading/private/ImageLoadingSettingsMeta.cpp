#include "ImageLoadingSettingsMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	rendering::image_loading::ImageLoadingSettingsMeta m_meta;
}

rendering::image_loading::ImageLoadingSettingsMeta::ImageLoadingSettingsMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const rendering::image_loading::ImageLoadingSettingsMeta& rendering::image_loading::ImageLoadingSettingsMeta::GetInstance()
{
	return m_meta;
}
