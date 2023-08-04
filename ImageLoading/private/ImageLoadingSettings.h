#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace rendering::image_loading
{
	class ImageLoadingSettings : public settings::SettingsReader
	{
	public:
		struct ImageEntry
		{
			std::string m_imageFile;
			std::string m_descriptionFile;
		};
		struct Settings
		{
			std::map<std::string, ImageEntry> m_images;
		};
	private:
		Settings m_settings;

		void LoadImageSettings();
	public:
		ImageLoadingSettings();
		virtual ~ImageLoadingSettings();

		Settings& GetSettings();
	};
}
