#pragma once

#include "ShadowMap.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>
#include <list>
#include <vector>

namespace rendering
{
	class DXTexture;
	class DXMutableBuffer;
}

namespace rendering::psm
{
	class PSM : public shadow_mapping::ShadowMap
	{
	public:
		static const UINT64 m_resolution;

	private:
		DXTexture* m_sm = nullptr;
		DXTexture* m_smDS = nullptr;
		DXTexture* m_shadowMask = nullptr;
		DXMutableBuffer* m_settingsBuffer = nullptr;

		void LoadSMTex(jobs::Job* done);
		void LoadSMDSTex(jobs::Job* done);
		void LoadShadowMaskTex(jobs::Job* done);
		void LoadSettingsBuffer(jobs::Job* done);

	public:
		PSM();
		virtual ~PSM();

		void LoadResources(jobs::Job* done) override;
		DXTexture* GetShadowMask() override;
		void RenderShadowMask() override;

		DXMutableBuffer* GetSettingsBuffer();
		void UpdateSMSettings();
	};
}