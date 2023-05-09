#pragma once

#include "Job.h"

namespace rendering
{
	class DXTexture;

	namespace deferred
	{
		DXTexture* GetGBufferDiffuseTex();
		DXTexture* GetGBufferNormalTex();
		DXTexture* GetGBufferPositionTex();

		void LoadGBuffer(jobs::Job* done);
	}
}