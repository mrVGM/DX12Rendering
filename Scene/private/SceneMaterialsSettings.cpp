#include "SceneMaterialsSettings.h"

#include "SceneMaterialsSettingsMeta.h"

#include "XMLReader.h"

rendering::SceneMaterialsSettings::SceneMaterialsSettings(const std::string& filename) :
	settings::SettingsReader(SceneMaterialsSettingsMeta::GetInstance()),
	m_filename(filename)
{
}

rendering::SceneMaterialsSettings::~SceneMaterialsSettings()
{
}

rendering::SceneMaterialsSettings::Settings& rendering::SceneMaterialsSettings::GetSettings()
{
	return m_settings;
}

void rendering::SceneMaterialsSettings::LoadSceneMaterialsSettings(std::map<std::string, collada::ColladaMaterial>& outMaterials)
{
	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(m_filename, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	for (auto it = settingsNode->m_children.begin(); it != settingsNode->m_children.end(); ++it)
	{
		xml_reader::Node* cur = *it;
		
		const xml_reader::Node* diffuse = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "diffuse";
		});

		outMaterials[cur->m_tagProps["id"]] = collada::ColladaMaterial();
		collada::ColladaMaterial& mat = outMaterials[cur->m_tagProps["id"]];
		mat.m_name = cur->m_tagProps["id"];

		int colorIndex = 0;
		for (auto dataIt = diffuse->m_data.begin(); dataIt != diffuse->m_data.end(); ++dataIt)
		{
			scripting::ISymbol* curSymbol = *dataIt;
			mat.m_diffuseColor[colorIndex++] = curSymbol->m_symbolData.m_number;
		}
	}
}
