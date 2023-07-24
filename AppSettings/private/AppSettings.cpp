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
	SettingsReader(settings::AppSettingsMeta::GetInstance())
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
	XMLNodes xmlNodes;
	ParseSettingFile(settingsPath, xmlNodes);

	xml_reader::Node* settingsNode = nullptr;

	for (auto it = xmlNodes.m_rootNodes.begin(); it != xmlNodes.m_rootNodes.end(); ++it)
	{
		xml_reader::Node* cur = *it;
		if (cur->m_tagName == "settings")
		{
			settingsNode = cur;
			break;
		}
	}

	if (!settingsNode)
	{
		throw "Can't Find the 'settings' Node!";
	}

	{
		std::list<const xml_reader::Node*> tmp;
		xml_reader::FindChildNodes(settingsNode, [](const xml_reader::Node* node) {
			if (node->m_tagName == "entry_point")
			{
				return true;
			}
			return false;
		}, tmp);

		if (tmp.size() > 0)
		{
			m_settings.m_appEntryPoint = tmp.front()->m_data.front()->m_symbolData.m_string;
		}
	}

	const xml_reader::Node* otherSettingsNode = nullptr;
	{
		std::list<const xml_reader::Node*> tmp;
		xml_reader::FindChildNodes(settingsNode, [](const xml_reader::Node* node) {
			if (node->m_tagName == "other_settings")
			{
				return true;
			}
			return false;
		}, tmp);

		if (tmp.size() > 0)
		{
			otherSettingsNode = tmp.front();
		}
	}

	if (otherSettingsNode)
	{
		for (auto it = otherSettingsNode->m_children.begin(); it != otherSettingsNode->m_children.end(); ++it)
		{
			xml_reader::Node* cur = *it;
			const std::string& name = cur->m_tagName;
			const std::string& path = cur->m_data.front()->m_symbolData.m_string;

			m_settings.m_otherSettings[name] = path;
		}
	}
}

const settings::AppSettings::Settings& settings::AppSettings::GetSettings() const
{
	return m_settings;
}

settings::AppSettings* settings::AppSettings::GetAppSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(AppSettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find AppSettings!";
	}

	AppSettings* settings = static_cast<AppSettings*>(obj);
	return settings;
}