#include "ShaderRepo.h"

#include "BaseObjectMeta.h"
#include "DXShader.h"

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

	rendering::DXShader* m_shadowMapVertexShader = nullptr;
	rendering::DXShader* m_shadowMapPixelShader = nullptr;
	rendering::DXShader* m_displayShadowMapPixelShader = nullptr;

	rendering::DXShader* m_shadowMaskPixelShader = nullptr;
	rendering::DXShader* m_shadowMaskPCFFilterPixelShader = nullptr;
	rendering::DXShader* m_shadowMaskDitherFilterPixelShader = nullptr;
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

rendering::DXShader* rendering::shader_repo::GetShadowMapVertexShader()
{
	return m_shadowMapVertexShader;
}

rendering::DXShader* rendering::shader_repo::GetShadowMapPixelShader()
{
	return m_shadowMapPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetDisplayShadowMapPixelShader()
{
	return m_displayShadowMapPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetShadowMaskPixelShader()
{
	return m_shadowMaskPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetShadowMaskPCFFilterPixelShader()
{
	return m_shadowMaskPCFFilterPixelShader;
}

rendering::DXShader* rendering::shader_repo::GetShadowMaskDitherFilterPixelShader()
{
	return m_shadowMaskDitherFilterPixelShader;
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

	m_shadowMapVertexShader = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_shadowMapVS.fxc");
	m_shadowMapPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_shadowMap.fxc");

	m_displayShadowMapPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_displayShadowMap.fxc");

	m_shadowMaskPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_shadowMaskPS.fxc");
	m_shadowMaskPCFFilterPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_shadowMaskPCFFilterPS.fxc");
	m_shadowMaskDitherFilterPixelShader = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_shadowMaskDitherFilterPS.fxc");
}
