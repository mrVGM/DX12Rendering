#include "SceneConverterSettings.h"

#include "SceneConverterSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

scene_converter::SceneConverterSettings::SceneConverterSettings() :
	settings::SettingsReader(SceneConverterSettingsMeta::GetInstance())
{
	LoadSceneSettings();
}

scene_converter::SceneConverterSettings::~SceneConverterSettings()
{
}

scene_converter::SceneConverterSettings::Settings& scene_converter::SceneConverterSettings::GetSettings()
{
	return m_settings;
}

void scene_converter::SceneConverterSettings::LoadSceneSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("scene_converter")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	const xml_reader::Node* scenesToConvert = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "scenes_to_convert";
	});

	for (auto it = scenesToConvert->m_children.begin(); it != scenesToConvert->m_children.end(); ++it)
	{
		xml_reader::Node* cur = *it;

		const xml_reader::Node* dae = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "dae";
		});

		const std::string& daePath = dae->m_data.front()->m_symbolData.m_string;
		SceneInfo si{ daePath };

		m_settings.m_scenes[cur->m_tagName] = si;
	}
	bool t = true;
}
