#pragma once

#include "SettingsReader.h"

#include <string>
#include <map>

namespace rendering
{
	class OutlineSettings : public settings::SettingsReader
	{
	public:

		struct Settings
		{
			float m_color[4];
			float m_scale;
		};
	private:
		Settings m_settings;

		void LoadOutlineSettings();
	public:
		OutlineSettings();
		virtual ~OutlineSettings();

		Settings& GetSettings();
	};
}
