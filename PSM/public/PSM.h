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
	class DXDescriptorHeap;
}

namespace rendering::psm
{
	class PSM : public shadow_mapping::ShadowMap
	{
	public:
		static const UINT64 m_resolution;

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_preSMRenderList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_postSMRenderList;

		DXTexture* m_sm = nullptr;
		DXTexture* m_smDS = nullptr;
		DXTexture* m_shadowMask = nullptr;
		DXMutableBuffer* m_settingsBuffer = nullptr;
		DXDescriptorHeap* m_smDescriptorHeap = nullptr;

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