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
	class DXDeferredRP : public RenderPass
	{
	public:
		enum GBufferTexType
		{
			Diffuse,
			Normal,
			Position
		};

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_endListAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_startListAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;

		UINT m_rtvDescriptorSize = 0;
		UINT m_srvDescriptorSize = 0;

		const DXShader& m_vertexShader;
		const DXShader& m_pixelShader;

		void CreateRTVHeap();
		void CreateSRVHeap();

		void PrepareEndList();
		void PrepareStartList();
	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferTexType texType);

		void Prepare() override;
		void Execute() override;
	};
}