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

	class DXDisplaySMRP : public RenderPass
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		DXShader& m_vertexShader;
		DXShader& m_pixelShader;

		void Create();
		void CreatePipelineStageAndRootSignature();
	public:
		DXDisplaySMRP();
		virtual ~DXDisplaySMRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}