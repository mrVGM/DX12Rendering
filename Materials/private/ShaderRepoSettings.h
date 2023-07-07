#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include "ShaderRepo.h"

#include <string>
#include <map>

namespace rendering
{
	class DXShader;

	class ShaderRepoSettings : public settings::SettingsReader
	{
	public:
		struct Settings
		{
			std::map<std::string, DXShader*> m_shaderMap;
			std::map<std::string, shader_repo::ShaderSet> m_shaderSets;
		};
	private:
		Settings m_settings;

		void LoadShaders();
	public:
		ShaderRepoSettings();
		virtual ~ShaderRepoSettings();

		DXShader* GetShaderByName(const std::string& name);
		const shader_repo::ShaderSet& GetShaderSetByName(const std::string& name);
	};
}
