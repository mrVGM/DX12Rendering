#pragma once

#include "Job.h"

namespace rendering
{
	class DXTexture;
	class DXBuffer;

	namespace deferred
	{
		DXTexture* GetGBufferDiffuseTex();
		DXTexture* GetGBufferSpecularTex();
		DXTexture* GetGBufferNormalTex();
		DXTexture* GetGBufferPositionTex();

		DXTexture* GetGBufferAmbientLitTex();
		DXTexture* GetGBufferDiffuseLitTex();
		DXTexture* GetGBufferSpecularLitTex();

		DXBuffer* GetLightsBuffer();
		DXBuffer* GetShadowMapTexture();

		DXBuffer* GetRenderTextureVertexBuffer();

		void LoadGBuffer(jobs::Job* done);
		void LoadGBufferLitTextures(jobs::Job* done);
	}
}