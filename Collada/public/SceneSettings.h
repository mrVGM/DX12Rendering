#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace collada
{
	class SceneSettings : public settings::SettingsReader
	{
	public:

		struct SceneInfo
		{
			std::string m_dae;
			std::string m_binFile;
			std::string m_materialsFile;
		};

		struct Settings
		{
			std::map<std::string, SceneInfo> m_scenes;
		};
	private:
		Settings m_settings;

		void LoadSceneSettings();
	public:
		SceneSettings();
		virtual ~SceneSettings();

		Settings& GetSettings();
	};
}
