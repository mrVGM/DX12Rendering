#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXDescriptorHeap;
	class DXTexture;
	class DXMutableBuffer;
}

namespace rendering
{
	class DXOutlineMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		ID3D12CommandList* m_precalculatedLists[2] = {};

		DXDescriptorHeap* m_srvHeap = nullptr;

		DXMutableBuffer* m_settingsBuffer = nullptr;

		void CreatePipelineStateAndRootSignature();
		void CreateSRVHeap();

	public:
		DXOutlineMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXOutlineMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;

		void LoadSettingsBuffer(jobs::Job* done);
	};
}