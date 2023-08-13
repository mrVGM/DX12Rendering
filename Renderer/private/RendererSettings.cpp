#include "RendererSettings.h"

#include "RendererSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

rendering::RendererSettings::RendererSettings() :
	settings::SettingsReader(RendererSettingsMeta::GetInstance())
{
	LoadRendererSettings();
}

rendering::RendererSettings::~RendererSettings()
{
}

rendering::RendererSettings::Settings& rendering::RendererSettings::GetSettings()
{
	return m_settings;
}

void rendering::RendererSettings::LoadRendererSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("renderer")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	const xml_reader::Node* scene = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "scene_to_load";
	});

	m_settings.m_sceneToLoad = scene->m_data.front()->m_symbolData.m_string;
}
