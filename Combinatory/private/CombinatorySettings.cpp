#include "CombinatorySettings.h"

#include "CombinatorySettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

combinatory::CombinatorySettings::CombinatorySettings() :
	settings::SettingsReader(CombinatorySettingsMeta::GetInstance())
{
	LoadSettings();
}

combinatory::CombinatorySettings::~CombinatorySettings()
{
}

void combinatory::CombinatorySettings::LoadSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("combinatory")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	const xml_reader::Node* width = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "width";
	});

	m_settings.m_width = width->m_data.front()->m_symbolData.m_number;
}
