#include "SettingsReader.h"

#include "DataLib.h"
#include "XMLReader.h"

settings::SettingsReader::XMLNodes::~XMLNodes()
{
	for (auto it = m_allNodes.begin(); it != m_allNodes.end(); ++it)
	{
		delete* it;
	}
}

settings::SettingsReader::SettingsReader(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
	xml_reader::Boot();
}

settings::SettingsReader::~SettingsReader()
{
}

void settings::SettingsReader::ParseSettingFile(const std::string& settingFilePath, XMLNodes& XMLNodes)
{
	data::DataLib& lib = data::GetLibrary();
	std::string settingsPath = lib.GetRootDir() + settingFilePath;

	xml_reader::IXMLReader* reader = xml_reader::GetReader();

	scripting::ISymbol* s = reader->ReadColladaFile(settingsPath);

	std::list<xml_reader::Node*> rootNodes;
	reader->ConstructColladaTree(s, XMLNodes.m_rootNodes , XMLNodes.m_allNodes);

	if (!s)
	{
		throw "Can't Read Settings File!";
	}
}