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
			float m_depthThreshold;
			float m_normalThreshold;
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
