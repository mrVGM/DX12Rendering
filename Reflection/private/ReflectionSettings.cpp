#include "ReflectionSettings.h"

#include "ReflectionSettingsMeta.h"

#include "AppSettings.h"

#include "XMLReader.h"
#include "XMLWriter.h"
#include "DataLib.h"
#include "AppEntryPoint.h"
#include "AppEntryPointMeta.h"

#include "RendererEntryPointMetaTag.h"
#include "SceneConverterEntryPointMetaTag.h"

#include "BaseObjectContainer.h"

#include "DataLib.h"

#include <fstream>

reflection::ReflectionSettings::ReflectionSettings() :
	SettingsReader(reflection::ReflectionSettingsMeta::GetInstance())
{
	ReadSettingFile();
}

reflection::ReflectionSettings::~ReflectionSettings()
{
}

void reflection::ReflectionSettings::StoreSettings()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("scripting_lib")->second;

	xml_writer::Node settingsNode;
	settingsNode.m_tagName = "settings";
	
	xml_writer::Node& dirNode = settingsNode.m_children.emplace_back();
	dirNode.m_tagName = "dir";
	dirNode.m_content = xml_writer::EncodeAsString(m_settings.m_dirPath);

	xml_writer::Node& filesNode = settingsNode.m_children.emplace_back();
	filesNode.m_tagName = "files";
	
	for (auto it = m_settings.m_files.begin(); it != m_settings.m_files.end(); ++it)
	{
		xml_writer::Node& fileNode = filesNode.m_children.emplace_back();
		fileNode.m_tagName = "file";
		fileNode.m_tagProps["id"] = it->first;
		fileNode.m_content = xml_writer::EncodeAsString(it->second);
	}


	{
		std::string xml = settingsNode.ToString();
		std::string reflectionSettingsFilePath = data::GetLibrary().GetRootDir() + settingsFile;

		std::ofstream reflectionFile(reflectionSettingsFilePath);
		reflectionFile << xml;
	}
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
			m_settings.m_files[cur->m_tagProps["id"]] = cur->m_data.front()->m_symbolData.m_string;
		}
	}
}

reflection::ReflectionSettings::Settings& reflection::ReflectionSettings::GetSettings()
{
	return m_settings;
}
