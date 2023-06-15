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
	m_appEntryPoints["Renderer"] = &RendererEntryPointMetaTag::GetInstance();
	m_appEntryPoints["SceneConverter"] = &SceneConverterEntryPointMetaTag::GetInstance();

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
			const BaseObjectMetaTag* tag = m_appEntryPoints.find(appEntryPointName)->second;

			m_settings.m_appEntryPointTag = tag;
			break;
		}
	}
}

const settings::AppSettings::Settings& settings::AppSettings::GetSettings() const
{
	return m_settings;
}

void settings::BootApp()
{

	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	const AppSettings* settings = nullptr;
	{
		BaseObject* tmp = container.GetObjectOfClass(settings::AppSettingsMeta::GetInstance());
		settings = static_cast<AppSettings*>(tmp);
	}
	
	std::list<BaseObject*> tmp;
	container.GetAllObjectsOfClass(settings::AppEntryPointMeta::GetInstance(), tmp);

	const BaseObjectMetaTag* entryTag = settings->GetSettings().m_appEntryPointTag;

	for (auto it = tmp.begin(); it != tmp.end(); ++it)
	{
		AppEntryPoint* entryPoint = static_cast<AppEntryPoint*>(*it);
		if (entryPoint->GetMeta().HasTag(*entryTag))
		{
			entryPoint->Boot();
			return;
		}
	}
}