#pragma once

#include "BaseObject.h"
#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace settings
{
	class AppSettings : public BaseObject
	{
	public:
		struct Settings
		{
			std::string m_sceneName;
			std::string m_appEntryPoint;
		};
	private:
		Settings m_settings;
		std::map<std::string, std::string> m_appEntryPoints;

		void ReadSettingFile();
	public:
		AppSettings();
		virtual ~AppSettings();

		const Settings& GetSettings() const;
	};
}
