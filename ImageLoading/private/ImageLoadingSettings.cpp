#include "ImageLoadingSettings.h"

#include "ImageLoadingSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

rendering::image_loading::ImageLoadingSettings::ImageLoadingSettings() :
	settings::SettingsReader(image_loading::ImageLoadingSettingsMeta::GetInstance())
{
	LoadImageSettings();
}

rendering::image_loading::ImageLoadingSettings::~ImageLoadingSettings()
{
}

rendering::image_loading::ImageLoadingSettings::Settings& rendering::image_loading::ImageLoadingSettings::GetSettings()
{
	return m_settings;
}

void rendering::image_loading::ImageLoadingSettings::LoadImageSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("image_loading")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	for (auto childIt = settingsNode->m_children.begin(); childIt != settingsNode->m_children.end(); ++childIt)
	{
		const xml_reader::Node* cur = *childIt;

		const xml_reader::Node* image = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "image";
		});

		const xml_reader::Node* description = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "description";
		});

		m_settings.m_images[cur->m_tagName] = ImageEntry{ image->m_data.front()->m_symbolData.m_string, description->m_data.front()->m_symbolData.m_string };
	}
}
