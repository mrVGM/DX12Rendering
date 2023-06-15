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
			const BaseObjectMetaTag* m_appEntryPointTag = nullptr;
		};
	private:
		Settings m_settings;
		std::map<std::string, const BaseObjectMetaTag*> m_appEntryPoints;

		void ReadSettingFile();
	public:
		AppSettings();
		virtual ~AppSettings();

		const Settings& GetSettings() const;
	};

	void BootApp();
}
