#include "AppSettings.h"

#include "AppSettingsMeta.h"

#include "XMLReader.h"
#include "DataLib.h"
#include "AppEntryPoint.h"
#include "AppEntryPointMeta.h"

#include "RendererEntryPointMetaTag.h"
#include "SceneConverterEntryPointMetaTag.h"

#include "BaseObjectContainer.h"

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

	for (auto it = rootNodes.begin(); it != rootNodes.end(); ++it)
	{
		std::list<const xml_reader::Node*> tmp;
		xml_reader::FindChildNodes(*it, [](const xml_reader::Node* node) {
			if (node->m_tagName == "entry_point")
			{
				return true;
			}
			return false;
		}, tmp);

		if (tmp.size() > 0)
		{
			const std::string& appEntryPointName = tmp.front()->m_data.front()->m_symbolData.m_string;
			m_settings.m_appEntryPoint = appEntryPointName;

			break;
		}
	}

	for (auto it = rootNodes.begin(); it != rootNodes.end(); ++it)
	{
		std::list<const xml_reader::Node*> tmp;
		xml_reader::FindChildNodes(*it, [](const xml_reader::Node* node) {
			if (node->m_tagName == "shadow_map_type")
			{
				return true;
			}
		return false;
			}, tmp);

		if (tmp.size() > 0)
		{
			const std::string& shadpwMapType = tmp.front()->m_data.front()->m_symbolData.m_string;
			m_settings.m_shadowMapType = shadpwMapType;

			break;
		}
	}

	for (auto it = rootNodes.begin(); it != rootNodes.end(); ++it)
	{
		std::list<const xml_reader::Node*> psms;
		xml_reader::FindChildNodes(*it, [](const xml_reader::Node* node) {
			if (node->m_tagName == "psm")
			{
				return true;
			}
			return false;
		}, psms);

		if (psms.size() > 0)
		{
			std::list<const xml_reader::Node*> near;
			xml_reader::FindChildNodes(psms.front(), [](const xml_reader::Node* node) {
				if (node->m_tagName == "near")
				{
					return true;
				}

				return false;
			}, near);

			if (near.size() > 0)
			{
				m_settings.m_psmNear = near.front()->m_data.front()->m_symbolData.m_number;
				break;
			}
		}
	}
}

const settings::AppSettings::Settings& settings::AppSettings::GetSettings() const
{
	return m_settings;
}


settings::AppSettings* settings::GetSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(AppSettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find App Settings!";
	}

	AppSettings* settings = static_cast<AppSettings*>(obj);

	return settings;
}