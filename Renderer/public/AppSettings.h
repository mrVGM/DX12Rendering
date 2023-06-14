#pragma once

#include "BaseObject.h"

#include <string>

namespace rendering
{
	class AppSettings : public BaseObject
	{
	public:
		struct Settings
		{
			std::string m_sceneName;
		};
	private:
		Settings m_settings;

		void ReadSettingFile();
	public:
		AppSettings();
		virtual ~AppSettings();

		const Settings& GetSettings() const;
	};
}
