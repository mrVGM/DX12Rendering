#pragma once

#include "DXMaterial.h"

namespace rendering
{
	class DXUnlitMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	public:
		DXUnlitMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXUnlitMaterial();

		virtual void GenerateCommandList(const DXBuffer& vertexBuffer, const DXBuffer& indexBuffer, const DXBuffer& instanceBuffer) override;
	};
}