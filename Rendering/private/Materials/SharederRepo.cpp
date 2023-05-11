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

	rendering::DXShader* m_deferredRPPixelShader = nullptr;
	rendering::DXShader* m_deferredRPVertexShader = nullptr;
	rendering::DXShader* m_deferredRPPostLightingPixelShader = nullptr;
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

rendering::DXShader* rendering::shader_repo::GetDeferredRPPixelShader()
{
	return m_deferredRPPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetDeferredRPVertexShader()
{
	return m_deferredRPVertexShader;
}

rendering::DXShader* rendering::shader_repo::GetDeferredRPPostLightingPixelShader()
{
	return m_deferredRPPostLightingPixelShader;
}

void rendering::shader_repo::LoadShaderPrograms()
{
	m_mainVertexShader = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_mainVS.fxc");
	m_errorPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_error.fxc");
	m_unlitPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_unlit.fxc");
	m_deferredPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_deferred.fxc");

	m_deferredRPVertexShader = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_deferredRPVS.fxc");
	m_deferredRPPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_deferredRPPS.fxc");
	m_deferredRPPostLightingPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_deferredRPPostLightingPS.fxc");
}
