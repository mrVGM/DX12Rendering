#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXBuffer;
}
namespace rendering::psm
{

	class DXShadowMapMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_straightLightPipelineState;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_invertedLightPipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	public:
		DXShadowMapMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXShadowMapMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;
	};
}