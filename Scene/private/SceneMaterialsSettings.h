#pragma once

#include "SettingsReader.h"

#include "ColladaEntities.h"

#include <string>
#include <map>

namespace rendering
{
	class SceneMaterialsSettings : public settings::SettingsReader
	{
	public:

		struct Settings
		{
			std::string m_sceneToLoad;
		};
	private:
		std::string m_filename;
		Settings m_settings;

	public:
		void LoadSceneMaterialsSettings(std::map<std::string, collada::ColladaMaterial>& outMaterials);

		SceneMaterialsSettings(const std::string& filename);
		virtual ~SceneMaterialsSettings();

		Settings& GetSettings();
	};
}
