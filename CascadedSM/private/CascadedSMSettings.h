#pragma once

#include "SettingsReader.h"

#include <string>

namespace rendering
{
	class CascadedSMSettings : public settings::SettingsReader
	{
	public:

		struct Settings
		{
			int m_resolution;
			std::list<float> m_bounds;
		};
	private:
		Settings m_settings;

		void LoadSettings();
	public:
		CascadedSMSettings();
		virtual ~CascadedSMSettings();

		Settings& GetSettings();
	};
}
