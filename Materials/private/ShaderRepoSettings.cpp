#include "ShaderRepoSettings.h"

#include "ShaderRepoSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

#include "DXShader.h"

#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"

rendering::ShaderRepoSettings::ShaderRepoSettings() :
	settings::SettingsReader(ShaderRepoSettingsMeta::GetInstance())
{
	LoadShaders();
}

rendering::ShaderRepoSettings::~ShaderRepoSettings()
{
}

rendering::DXShader* rendering::ShaderRepoSettings::GetShaderByName(const std::string& name)
{
	auto it = m_settings.m_shaderMap.find(name);

	if (it == m_settings.m_shaderMap.end())
	{
		return nullptr;
	}

	return it->second;
}

const rendering::shader_repo::ShaderSet& rendering::ShaderRepoSettings::GetShaderSetByName(const std::string& name)
{
	return m_settings.m_shaderSets[name];
}

void rendering::ShaderRepoSettings::LoadShaders()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("shader_repo")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);

	const xml_reader::Node* vertexShaders = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "vertex_shaders";
	});

	const xml_reader::Node* pixelShaders = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "pixel_shaders";
	});

	const xml_reader::Node* shaderSets = xml_reader::FindChildNode(settingsNode, [](const xml_reader::Node* node) {
		return node->m_tagName == "shader_sets";
	});

	for (auto it = vertexShaders->m_children.begin(); it != vertexShaders->m_children.end(); ++it)
	{
		const xml_reader::Node* cur = *it;
		std::string name = cur->m_tagName;
		std::string path = cur->m_data.front()->m_symbolData.m_string;

		DXShader* shader = new DXShader(DXVertexShaderMeta::GetInstance(), path);
		m_settings.m_shaderMap[name] = shader;
	}

	for (auto it = pixelShaders->m_children.begin(); it != pixelShaders->m_children.end(); ++it)
	{
		const xml_reader::Node* cur = *it;
		std::string name = cur->m_tagName;
		std::string path = cur->m_data.front()->m_symbolData.m_string;

		DXShader* shader = new DXShader(DXPixelShaderMeta::GetInstance(), path);
		m_settings.m_shaderMap[name] = shader;
	}

	for (auto it = shaderSets->m_children.begin(); it != shaderSets->m_children.end(); ++it)
	{
		const xml_reader::Node* cur = *it;

		std::string name = cur->m_tagName;
		shader_repo::ShaderSet& shaderSet = m_settings.m_shaderSets[name];

		const xml_reader::Node* vertexShader = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "vertex";
		});
		if (vertexShader)
		{
			std::string shaderName = vertexShader->m_data.front()->m_symbolData.m_string;
			shaderSet.m_vertexShader = m_settings.m_shaderMap[shaderName];
		}

		const xml_reader::Node* pixelShader = xml_reader::FindChildNode(cur, [](const xml_reader::Node* node) {
			return node->m_tagName == "pixel";
		});
		if (pixelShader)
		{
			std::string shaderName = pixelShader->m_data.front()->m_symbolData.m_string;
			shaderSet.m_pixelShader = m_settings.m_shaderMap[shaderName];
		}
	}
}
