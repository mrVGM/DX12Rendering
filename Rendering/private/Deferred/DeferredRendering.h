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

		DXBuffer* GetRenderTextureVertexBuffer();

		void LoadGBuffer(jobs::Job* done);
	}
}