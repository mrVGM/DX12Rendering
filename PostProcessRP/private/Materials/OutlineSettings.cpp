#include "OutlineSettings.h"

#include "OutlineSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

rendering::OutlineSettings::OutlineSettings() :
	settings::SettingsReader(OutlineSettingsMeta::GetInstance())
{
	LoadOutlineSettings();
}

rendering::OutlineSettings::~OutlineSettings()
{
}

rendering::OutlineSettings::Settings& rendering::OutlineSettings::GetSettings()
{
	return m_settings;
}

void rendering::OutlineSettings::LoadOutlineSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("outline")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	{
		const xml_reader::Node* color = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			return node->m_tagName == "color";
		});

		int index = 0;
		for (auto it = color->m_data.begin(); it != color->m_data.end(); ++it)
		{
			scripting::ISymbol* cur = *it;
			m_settings.m_color[index++] = cur->m_symbolData.m_number;

			if (index >= 4)
			{
				break;
			}
		}
	}

	{
		const xml_reader::Node* scale = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			return node->m_tagName == "scale";
		});

		m_settings.m_scale = scale->m_data.front()->m_symbolData.m_number;
	}

	{
		const xml_reader::Node* depthThreshold = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			return node->m_tagName == "depth_threshold";
		});

		m_settings.m_depthThreshold = depthThreshold->m_data.front()->m_symbolData.m_number;
	}
}
