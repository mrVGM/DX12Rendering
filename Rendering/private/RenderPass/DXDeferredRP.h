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

	class DXDeferredRP : public RenderPass
	{
	public:
		enum GBufferTexType
		{
			Diffuse = 0,
			Specular = 1,
			Normal = 2,
			Position = 3,
			ShadowMap = 4,
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
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvLitHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvLitHeap;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_postlightCalculationsAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_postLightingList;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_postLigtingPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_postLigtingRootSignature;

		DXBuffer* m_lightsBuffer = nullptr;
		DXTexture* m_shadowMapTex = nullptr;

		UINT m_rtvDescriptorSize = 0;
		UINT m_srvDescriptorSize = 0;

		bool m_startListPrepared = false;
		bool m_endListPrepared = false;

		const DXShader& m_vertexShader;
		const DXShader& m_pixelShader;

		const DXShader& m_postLightingVertexShader;
		const DXShader& m_postLightingPixelShader;

		void CreateLightCalculationsPipelineStageAndRootSignature();
		void CreatePostLightingPipelineStageAndRootSignature();

		void CreateRTVHeap();
		void CreateSRVHeap();

		void CreateRTVLitHeap();
		void CreateSRVLitHeap();

		void PrepareEndList();
		void PrepareStartList();
		void PreparePostLightingList();

		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);
		void LoadLitTextures(jobs::Job* done);

		void RenderDeferred();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferLitTexType texType);
	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferTexType texType);

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}