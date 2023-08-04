#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering::overlay
{
	class DXDescriptorHeap;
	class DXTexture;

	class DXDisplayTextMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		void CreatePipelineStateAndRootSignature();
		void CreateDescriptorHeaps();

	public:
		DXDisplayTextMaterial(const DXShader& vertexShader, const DXShader& pixelShader, DXTexture* texture);
		virtual ~DXDisplayTextMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;
	};
}