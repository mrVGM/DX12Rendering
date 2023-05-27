#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <vector>

namespace rendering
{
	class DXBuffer;
	class DXTexture;
	class DXDescriptorHeap;

	struct Light
	{
		float m_direction[3];
		float m_range;
	};

	class LightsManager : public BaseObject
	{
	public:
		static int m_shadowMapResolution;
	private:

		std::vector<Light> m_lights;
		DXBuffer* m_lightsBuffer = nullptr;
		DXTexture* m_shadowMap = nullptr;
		DXTexture* m_shadowMapDepthStencil = nullptr;
		DXBuffer* m_shadowMapSettingsBuffer = nullptr;

		rendering::DXDescriptorHeap* m_shadowMapDSDescriptorHeap = nullptr;
		rendering::DXDescriptorHeap* m_shadowMapRTV = nullptr;
		rendering::DXDescriptorHeap* m_shadowMapSRV = nullptr;

		void LoadShadowMapTex(jobs::Job* done);
		void LoadShadowMapDSTex(jobs::Job* done);
		void LoadShadowMapSettingsBuffer(jobs::Job* done);

		void CreateDescriptorHeaps();
	public:
		LightsManager();
		virtual ~LightsManager();

		void AddLight(const Light& light);
		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);

		void UpdateShadowMapSettings();

		DXBuffer* GetLightsBuffer();
		DXTexture* GetShadowMap();
		DXTexture* GetShadowMapDepthStencil();
		
		DXDescriptorHeap* GetShadowMapDSDescriptorHeap();
		DXDescriptorHeap* GetSMRTVHeap();
		DXDescriptorHeap* GetSMSRVHeap();
		DXBuffer* GetSMSettingsBuffer();

		const Light& GetLight(int index) const;
	};
}