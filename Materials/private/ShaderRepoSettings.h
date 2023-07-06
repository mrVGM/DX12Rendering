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
		struct Settings
		{
			std::map<std::string, DXShader*> m_shaders;
		};
	private:
		Settings m_settings;

		void LoadShaders();
	public:
		ShaderRepoSettings();
		virtual ~ShaderRepoSettings();
	};
}
