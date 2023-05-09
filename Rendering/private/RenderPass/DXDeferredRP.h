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
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		const DXShader& m_vertexShader;
		const DXShader& m_pixelShader;

	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBufferTexType texType);

		void Prepare() override;
		void Execute() override;
	};
}