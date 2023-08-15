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
			float m_angleFactor;
			float m_distanceLimits[2];
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
