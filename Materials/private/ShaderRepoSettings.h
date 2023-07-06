#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace rendering
{
	class DXShader;

	class ShaderRepoSettings : public settings::SettingsReader
	{
	public:
		struct ShaderSet
		{
			DXShader* m_vertexShader = nullptr;
			DXShader* m_pixelShader = nullptr;
		};

		struct Settings
		{
			std::map<std::string, DXShader*> m_shaderMap;
			std::map<std::string, ShaderSet> m_shaderSets;
		};
	private:
		Settings m_settings;

		void LoadShaders();
	public:
		ShaderRepoSettings();
		virtual ~ShaderRepoSettings();
	};
}
