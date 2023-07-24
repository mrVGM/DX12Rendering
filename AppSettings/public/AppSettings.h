#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace settings
{
	class AppSettings : public SettingsReader
	{
	public:
		struct Settings
		{
			std::string m_appEntryPoint;
			std::map<std::string, std::string> m_otherSettings;
		};
	private:
		Settings m_settings;
		std::map<std::string, std::string> m_appEntryPoints;

		void ReadSettingFile();
	public:
		AppSettings();
		virtual ~AppSettings();

		static AppSettings* GetAppSettings();

		const Settings& GetSettings() const;
	};
}
