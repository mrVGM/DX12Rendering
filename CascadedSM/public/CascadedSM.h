#pragma once

#include "ShadowMap.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>
#include <list>
#include <vector>

namespace rendering
{
	class DXBuffer;
	class DXMutableBuffer;
	class DXTexture;
	class DXDescriptorHeap;
	class DXMaterial;
	class DXShadowMapMaterial;

	class CascadedSM : public shadow_mapping::ShadowMap
	{
	private:
		bool m_listsDirty = true;
		int m_numCommandLists = 0;
		ID3D12CommandList** m_commandListsCache = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_preSMRenderList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_postSMRenderList;

		DXMutableBuffer* m_smSettingsBuffer = nullptr;
		DXTexture* m_smFilterTex = nullptr;

		std::vector<DXTexture*> m_smTex;
		std::vector<DXTexture*> m_shadowMaskTex;

		std::list<DXShadowMapMaterial*> m_shadowMapMaterials;

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

		bool m_preSMRenderListPrepared = false;
		bool m_postSMRenderListPrepared = false;
		void PreparePreSMRenderList();
		void PreparePostSMRenderList();

		void RenderScene();

	public:
		CascadedSM();
		virtual ~CascadedSM();

		void LoadResources(jobs::Job* done) override;
		DXTexture* GetShadowMask() override;
		void RenderShadowMask() override;

		void UpdateSMSettings();
		DXTexture* GetShadowMap(int index);
		DXTexture* GetShadowMapFilterTex();
		DXTexture* GetShadowMask(int index);
		DXDescriptorHeap* GetDSDescriptorHeap();
		DXDescriptorHeap* GetSMDescriptorHeap();
		DXMutableBuffer* GetSettingsBuffer();

		const std::list<DXShadowMapMaterial*>& GetShadowMapMaterials();

		void SetListsDirty();
	};
}