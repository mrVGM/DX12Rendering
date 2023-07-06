#include "ShaderRepoSettings.h"

#include "ShaderRepoSettingsMeta.h"

#include "XMLReader.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"

rendering::ShaderRepoSettings::ShaderRepoSettings() :
	settings::SettingsReader(ShaderRepoSettingsMeta::GetInstance())
{
	LoadShaders();
}

rendering::ShaderRepoSettings::~ShaderRepoSettings()
{
}

void rendering::ShaderRepoSettings::LoadShaders()
{
	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();
	const settings::AppSettings::Settings& settings = appSettings->GetSettings();

	std::string settingsFile = settings.m_otherSettings.find("shader_repo")->second;

	settings::SettingsReader::XMLNodes nodes;
	ParseSettingFile(settingsFile, nodes);

	const xml_reader::Node* settingsNode = FindSettingRootNode(nodes);


}
