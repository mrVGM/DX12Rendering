#include "AppSettings.h"

#include "AppSettingsMeta.h"

#include "XMLReader.h"

#include "DataLib.h"

settings::AppSettings::AppSettings() :
	BaseObject(settings::AppSettingsMeta::GetInstance())
{
	xml_reader::Boot();

	ReadSettingFile();
}

settings::AppSettings::~AppSettings()
{
}

void settings::AppSettings::ReadSettingFile()
{
	data::DataLib& lib = data::GetLibrary();

	std::string settingsPath = lib.GetFileEntry("settings").value("path", "");
	settingsPath = lib.GetRootDir() + settingsPath;

	xml_reader::IXMLReader* reader = xml_reader::GetReader();

	scripting::ISymbol* s = reader->ReadColladaFile(settingsPath);

	struct XMLNodes
	{
		std::list<xml_reader::Node*> m_allNodes;
		~XMLNodes()
		{
			for (auto it = m_allNodes.begin(); it != m_allNodes.end(); ++it)
			{
				delete *it;
			}
		}
	};

	XMLNodes nodes;
	std::list<xml_reader::Node*> rootNodes;
	reader->ConstructColladaTree(s, rootNodes, nodes.m_allNodes);

	if (!s)
	{
		throw "Can't Read Settings File!";
	}

	for (auto it = rootNodes.begin(); it != rootNodes.end(); ++it)
	{
		std::list<const xml_reader::Node*> tmp;
		xml_reader::FindChildNodes(*it, [](const xml_reader::Node* node) {
			if (node->m_tagName == "scene")
			{
				return true;
			}
			return false;
		}, tmp);

		if (tmp.size() > 0)
		{
			m_settings.m_sceneName = tmp.front()->m_data.front()->m_symbolData.m_string;
			break;
		}
	}
}

const settings::AppSettings::Settings& settings::AppSettings::GetSettings() const
{
	return m_settings;
}