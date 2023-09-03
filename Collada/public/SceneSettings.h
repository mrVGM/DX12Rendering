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

		struct AnimationInfo
		{
			std::string m_dae;
			std::string m_binFile;
		};

		struct Settings
		{
			std::map<std::string, SceneInfo> m_scenes;
			std::map<std::string, AnimationInfo> m_animations;
			std::list<std::string> m_scenesToConvert;
			std::list<std::string> m_animationsToConvert;
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
