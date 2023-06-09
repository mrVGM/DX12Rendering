#pragma once

#include "ShadowMap.h"

#include "Job.h"

#include <d3d12.h>

#include <list>
#include <vector>

namespace rendering
{
	class DXBuffer;
	class DXTexture;
	class DXDescriptorHeap;
	class DXMaterial;

	class CascadedSM : public ShadowMap
	{
	public:
		static const UINT m_resolution;

	private:
		const float m_cascadeSeparators[3] = { 20, 50, 200 };

		DXBuffer* m_smSettingsBuffer = nullptr;
		DXTexture* m_smFilterTex = nullptr;

		std::vector<DXTexture*> m_smTex;
		std::vector<DXTexture*> m_shadowMaskTex;

		std::list<DXMaterial*> m_shadowMapMaterials;

		std::list<DXTexture*> m_depthTextures;
		DXDescriptorHeap* m_dsDescriptorHeap = nullptr;
		DXDescriptorHeap* m_smDescriptorHeap = nullptr;

		void LoadSettingsBuffer(jobs::Job* done);
		void LoadDepthTextures(jobs::Job* done);
		void LoadSMTexture(jobs::Job* done);
		void LoadSMTextures(jobs::Job* done);
		void LoadSMFilterTexture(jobs::Job* done);
		void LoadShadowMaskTexture(jobs::Job* done);
		void LoadSMMaterials(jobs::Job* done);

		void CreateDescriptorHeaps();

	public:
		CascadedSM();
		virtual ~CascadedSM();

		void LoadResources(jobs::Job* done) override;
		DXTexture* GetShadowMask() override;
		void RenderShadowMask() override;

		void UpdateSMSettings();
		DXTexture* GetShadowMap(int index);
		DXTexture* GetShadowSQMap();
		DXTexture* GetShadowMapFilterTex();
		DXTexture* GetShadowMask(int index);
		DXDescriptorHeap* GetDSDescriptorHeap();
		DXDescriptorHeap* GetSMDescriptorHeap();
		DXBuffer* GetSettingsBuffer();

		const std::list<DXMaterial*>& GetShadowMapMaterials();
	};
}