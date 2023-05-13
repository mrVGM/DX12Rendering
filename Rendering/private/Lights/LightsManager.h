#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <list>

namespace rendering
{
	class DXBuffer;
	class DXTexture;
	class DXDescriptorHeap;

	struct Light
	{
		float m_position[3];
		float m_range;
	};

	class LightsManager : public BaseObject
	{
	private:
		std::list<Light> m_lights;
		DXBuffer* m_lightsBuffer = nullptr;
		DXTexture* m_shadowMap = nullptr;
		DXTexture* m_shadowMapDepthStencil = nullptr;
		rendering::DXDescriptorHeap* m_shadowMapDSDescriptorHeap = nullptr;

		void LoadShadowMapTex(jobs::Job* done);
		void LoadShadowMapDSTex(jobs::Job* done);
	public:
		LightsManager();
		virtual ~LightsManager();

		void AddLight(const Light& light);
		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);

		DXBuffer* GetLightsBuffer();
		DXTexture* GetShadowMap();
		DXTexture* GetShadowMapDepthStencil();
		DXDescriptorHeap* GetShadowMapDSDescriptorHeap();
	};
}