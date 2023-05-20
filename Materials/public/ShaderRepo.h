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
		DXShader* GetMainVertexShader();
		DXShader* GetErrorPixelShader();
		DXShader* GetUnlitPixelShader();
		DXShader* GetDeferredPixelShader();

		DXShader* GetDeferredRPPixelShader();
		DXShader* GetDeferredRPVertexShader();
		DXShader* GetDeferredRPPostLightingPixelShader();

		DXShader* GetShadowMapVertexShader();
		DXShader* GetShadowMapPixelShader();
		DXShader* GetDisplayShadowMapPixelShader();

		void LoadShaderPrograms();
	}
}