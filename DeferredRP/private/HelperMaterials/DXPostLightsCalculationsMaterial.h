#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXDescriptorHeap;

	class DXPostLightsCalculationsMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		DXDescriptorHeap* m_srvHeap = nullptr;

		void CreatePipelineStateAndRootSignature();
		void CreateDescriptorHeaps();

	public:
		DXPostLightsCalculationsMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXPostLightsCalculationsMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;
	};
}