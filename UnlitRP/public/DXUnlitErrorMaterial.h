#pragma once

#include "DXMaterial.h"

namespace rendering
{
	class DXUnlitErrorMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	public:
		DXUnlitErrorMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXUnlitErrorMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;
	};
}