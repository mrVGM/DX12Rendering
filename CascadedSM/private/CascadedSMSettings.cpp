#include "CascadedSMSettings.h"

#include "CascadedSMSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

rendering::CascadedSMSettings::CascadedSMSettings() :
	settings::SettingsReader(CascadedSMSettingsMeta::GetInstance())
{
	LoadSettings();
}

rendering::CascadedSMSettings::~CascadedSMSettings()
{
}

rendering::CascadedSMSettings::Settings& rendering::CascadedSMSettings::GetSettings()
{
	return m_settings;
}

void rendering::CascadedSMSettings::LoadSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("cascaded_sm")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	{
		const xml_reader::Node* resolution = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			return node->m_tagName == "resolution";
		});

		m_settings.m_resolution = static_cast<int>(resolution->m_data.front()->m_symbolData.m_number);
	}

	{
		const xml_reader::Node* bounds = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			return node->m_tagName == "bounds";
		});

		for (auto it = bounds->m_data.begin(); it != bounds->m_data.end(); ++it)
		{
			scripting::ISymbol* cur = *it;
			m_settings.m_bounds.push_back(cur->m_symbolData.m_number);
		}
	}
}
