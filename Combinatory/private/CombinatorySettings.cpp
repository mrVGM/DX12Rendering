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

	const xml_reader::Node* items = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "items";
	});

	for (auto it = items->m_children.begin(); it != items->m_children.end(); ++it)
	{
		const xml_reader::Node* itemWidth = xml_reader::FindChildNode(*it, [](const xml_reader::Node* node) {
			return node->m_tagName == "width";
		});

		const xml_reader::Node* itemLength = xml_reader::FindChildNode(*it, [](const xml_reader::Node* node) {
			return node->m_tagName == "length";
		});

		const xml_reader::Node* itemCount = xml_reader::FindChildNode(*it, [](const xml_reader::Node* node) {
			return node->m_tagName == "count";
		});

		Item cur;
		cur.m_width = itemWidth->m_data.front()->m_symbolData.m_number;
		cur.m_length = itemLength->m_data.front()->m_symbolData.m_number;
		cur.m_count = itemCount->m_data.front()->m_symbolData.m_number;

		m_settings.m_items.push_back(cur);
	}

}
