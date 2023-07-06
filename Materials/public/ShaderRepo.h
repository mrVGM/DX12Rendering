#pragma once

#include "DXMaterial.h"

#include "Job.h"

#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXShader;
	namespace shader_repo
	{
		struct ShaderSet
		{
			DXShader* m_vertexShader = nullptr;
			DXShader* m_pixelShader = nullptr;
		};

		DXShader* GetUnlitPixelShader();
		DXShader* GetDeferredPixelShader();

		DXShader* GetDeferredRPPixelShader();
		DXShader* GetDeferredRPVertexShader();
		DXShader* GetDeferredRPPostLightingPixelShader();

		DXShader* GetShadowMapVertexShader();
		DXShader* GetShadowMapPixelShader();
		DXShader* GetDisplayShadowMapPixelShader();

		DXShader* GetShadowMaskPixelShader();

		DXShader* GetShadowMaskPCFFilterPixelShader();
		DXShader* GetShadowMaskDitherFilterPixelShader();
		
		DXShader* GetIdentityFilterPixelShader();
		DXShader* GetGaussBlurFilterPixelShader();

		DXShader* GetEdgeOutlinePixelShader();

		void LoadShaderPrograms();
		DXShader* GetShaderByName(const std::string& name);
		const ShaderSet& GetShaderSetByName(const std::string& name);
	}
}