#include "SharederRepo.h"

#include "BaseObjectMeta.h"
#include "DXShader.h"

#include "RenderUtils.h"

#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"

namespace
{
	rendering::DXShader* m_mainVertexShader = nullptr;
	rendering::DXShader* m_errorPixelShader = nullptr;
	rendering::DXShader* m_unlitPixelShader = nullptr;
	rendering::DXShader* m_deferredPixelShader = nullptr;
}

rendering::DXShader* rendering::shader_repo::GetMainVertexShader()
{
	return m_mainVertexShader;
}

rendering::DXShader* rendering::shader_repo::GetErrorPixelShader()
{
	return m_errorPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetUnlitPixelShader()
{
	return m_unlitPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetDeferredPixelShader()
{
	return m_deferredPixelShader;
}

void rendering::shader_repo::LoadShaderPrograms()
{
	m_mainVertexShader = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_mainVS.fxc");
	m_errorPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_error.fxc");
	m_unlitPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_unlit.fxc");
	m_deferredPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_deferred.fxc");
}
