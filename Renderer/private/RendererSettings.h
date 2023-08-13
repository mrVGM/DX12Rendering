#pragma once

#include "SettingsReader.h"

#include <string>

namespace rendering
{
	class RendererSettings : public settings::SettingsReader
	{
	public:

		struct Settings
		{
			std::string m_sceneToLoad;
		};
	private:
		Settings m_settings;

		void LoadRendererSettings();
	public:
		RendererSettings();
		virtual ~RendererSettings();

		Settings& GetSettings();
	};
}
