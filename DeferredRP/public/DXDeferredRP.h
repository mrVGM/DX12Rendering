#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXShader;
	class DXBuffer;
	class DXTexture;
	class DXMaterial;
	class DXDescriptorHeap;

	class DXDeferredRP : public RenderPass
	{
	public:
		enum GBufferTexType
		{
			Diffuse = 0,
			Specular = 1,
			Normal = 2,
			Position = 3,
			ShadowMap = 4
		};

	private:
		ID3D12CommandList** m_commandListsCache = nullptr;
		int m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;

		DXDescriptorHeap* m_rtvHeap;
		DXDescriptorHeap* m_srvHeap;

		DXMaterial* m_shadowMapMaterial = nullptr;

		bool m_startListPrepared = false;
		
		void CreateRTVHeap();
		void CreateSRVHeap();

		void PrepareStartList();

		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);

		void RenderShadowMap();
		void RenderDeferred();

	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferTexType texType);

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}