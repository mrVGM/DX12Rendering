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

		enum GBufferLitTexType
		{
			AmbientLit = 0,
			DiffuseLit = 1,
			SpecularLit = 2
		};

	private:
		ID3D12CommandList** m_commandListsCache = nullptr;
		int m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_lightCalculationsAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;

		DXDescriptorHeap* m_rtvHeap;
		DXDescriptorHeap* m_srvHeap;

		DXDescriptorHeap* m_rtvLitHeap;
		DXDescriptorHeap* m_srvLitHeap;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_postlightCalculationsAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_postLightingList;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_postLigtingPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_postLigtingRootSignature;

		DXBuffer* m_lightsBuffer = nullptr;
		DXTexture* m_shadowMapTex = nullptr;

		DXMaterial* m_shadowMapMaterial = nullptr;

		UINT m_rtvDescriptorSize = 0;
		UINT m_srvDescriptorSize = 0;

		bool m_startListPrepared = false;
		bool m_endListPrepared = false;

		const DXShader& m_postLightingVertexShader;
		const DXShader& m_postLightingPixelShader;

		void CreatePostLightingPipelineStageAndRootSignature();

		void CreateRTVHeap();
		void CreateSRVHeap();

		void CreateRTVLitHeap();
		void CreateSRVLitHeap();

		void PrepareStartList();
		void PreparePostLightingList();

		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);

		void RenderShadowMap();
		void RenderDeferred();

	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferTexType texType);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferLitTexType texType);

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}