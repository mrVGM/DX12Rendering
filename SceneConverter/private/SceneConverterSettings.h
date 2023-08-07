#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace scene_converter
{
	class SceneConverterSettings : public settings::SettingsReader
	{
	public:

		struct SceneInfo
		{
			std::string m_dae;
		};

		struct Settings
		{
			std::map<std::string, SceneInfo> m_scenes;
		};
	private:
		Settings m_settings;

		void LoadSceneSettings();
	public:
		SceneConverterSettings();
		virtual ~SceneConverterSettings();

		Settings& GetSettings();
	};
}
