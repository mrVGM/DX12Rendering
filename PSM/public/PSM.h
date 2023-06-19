#pragma once

#include "ShadowMap.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>
#include <list>
#include <vector>
#include <DirectXMath.h>

namespace rendering
{
	class DXTexture;
	class DXMutableBuffer;
	class DXDescriptorHeap;
}

namespace rendering::psm
{
	class PSM : public shadow_mapping::ShadowMap
	{
	public:
		static const UINT64 m_resolution;

	private:
		ID3D12CommandList** m_commandListsCache = nullptr;
		UINT64 m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_preSMRenderList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_postSMRenderList;

		DXTexture* m_sm = nullptr;
		DXTexture* m_smDS = nullptr;
		DXTexture* m_shadowMask = nullptr;
		DXMutableBuffer* m_settingsBuffer = nullptr;
		DXDescriptorHeap* m_smDescriptorHeap = nullptr;
		DXDescriptorHeap* m_smDSDescriptorHeap = nullptr;

		void LoadSMTex(jobs::Job* done);
		void LoadSMDSTex(jobs::Job* done);
		void LoadShadowMaskTex(jobs::Job* done);
		void LoadSettingsBuffer(jobs::Job* done);

		void LoadResourcesInternal(jobs::Job* done);

		void CreateDescriptorHeap();

		bool m_preSMRenderListPrepared = false;
		bool m_postSMRenderListPrepared = false;
		void PreparePreSMRenderList();
		void PreparePostSMRenderList();

		DirectX::XMVECTOR GetLightPerspectiveOrigin();

		void RenderScene();
	public:
		PSM();
		virtual ~PSM();

		void LoadResources(jobs::Job* done) override;
		DXTexture* GetShadowMask() override;
		void RenderShadowMask() override;

		DXTexture* GetShadowMap(int index) override;
		DXDescriptorHeap* GetSMDescriptorHeap();
		DXDescriptorHeap* GetSMDSDescriptorHeap();
		DXMutableBuffer* GetSettingsBuffer();
		void UpdateSMSettings();
	};
}