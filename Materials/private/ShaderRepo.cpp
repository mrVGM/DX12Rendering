#include "ShaderRepo.h"

#include "BaseObjectMeta.h"
#include "DXShader.h"

#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"

#include "ShaderRepoSettings.h"

namespace
{
	rendering::ShaderRepoSettings* m_shaderRepoSettings = nullptr;
}

void rendering::shader_repo::LoadShaderPrograms()
{
	if (!m_shaderRepoSettings)
	{
		m_shaderRepoSettings = new rendering::ShaderRepoSettings();
	}
}

rendering::DXShader* rendering::shader_repo::GetShaderByName(const std::string& name)
{
	return m_shaderRepoSettings->GetShaderByName(name);
}

const rendering::shader_repo::ShaderSet& rendering::shader_repo::GetShaderSetByName(const std::string& name)
{
	return m_shaderRepoSettings->GetShaderSetByName(name);
}