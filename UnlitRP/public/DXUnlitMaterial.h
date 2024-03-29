#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXUnlitMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		DXBuffer* m_settingsBuffer = nullptr;

	public:
		DXUnlitMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXUnlitMaterial();

		void CreateSettingsBuffer(jobs::Job* done);

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;

		DXBuffer* GetSettingsBuffer();
	};
}