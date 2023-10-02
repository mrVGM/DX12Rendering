#include "ReflectionSettings.h"

#include "ReflectionSettingsMeta.h"

#include "AppSettings.h"

#include "XMLReader.h"
#include "DataLib.h"
#include "AppEntryPoint.h"
#include "AppEntryPointMeta.h"

#include "RendererEntryPointMetaTag.h"
#include "SceneConverterEntryPointMetaTag.h"

#include "BaseObjectContainer.h"

reflection::ReflectionSettings::ReflectionSettings() :
	SettingsReader(reflection::ReflectionSettingsMeta::GetInstance())
{
	ReadSettingFile();
}

reflection::ReflectionSettings::~ReflectionSettings()
{
}

void reflection::ReflectionSettings::ReadSettingFile()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("scripting_lib")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	{
		const xml_reader::Node* dirNode = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			if (node->m_tagName == "dir")
			{
				return true;
			}
			return false;
		});

		std::string path = dirNode->m_data.front()->m_symbolData.m_string;
		m_settings.m_dirPath = path;
	}

	{
		const xml_reader::Node* filesNode = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
			if (node->m_tagName == "files")
			{
				return true;
			}
			return false;
		});

		for (auto it = filesNode->m_children.begin(); it != filesNode->m_children.end(); ++it)
		{
			xml_reader::Node* cur = *it;
			m_settings.m_files[cur->m_tagName] = cur->m_data.front()->m_symbolData.m_string;
		}
	}
}

const reflection::ReflectionSettings::Settings& reflection::ReflectionSettings::GetSettings() const
{
	return m_settings;
}
