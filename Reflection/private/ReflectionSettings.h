#pragma once

#include "SettingsReader.h"

#include <string>
#include <map>

namespace reflection
{
	class ReflectionSettings : public settings::SettingsReader
	{
	public:
		struct Settings
		{
			std::string m_dirPath;
			std::map<std::string, std::string> m_files;
		};
	private:
		Settings m_settings;

		void ReadSettingFile();
	public:
		ReflectionSettings();
		virtual ~ReflectionSettings();

		const Settings& GetSettings() const;
	};
}
