#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXDescriptorHeap;
	class DXMutableBuffer;

	class DXDeferredMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		DXDescriptorHeap* m_rtvHeap = nullptr;
		DXMutableBuffer* m_settingsBuffer = nullptr;

		void CreateRTVHeap();
	public:
		DXDeferredMaterial(const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXDeferredMaterial();

		void CreateSettingsBuffer(jobs::Job* done);

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;

		DXMutableBuffer* GetSettingsBuffer();
	};
}