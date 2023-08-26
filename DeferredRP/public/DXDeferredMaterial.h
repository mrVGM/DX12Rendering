#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXDescriptorHeap;

	class DXDeferredMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineStateForSkeletalMesh;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignatureForSkeletalMesh;

		const DXShader& m_vertexShaderForSkeletalMesh;

		DXDescriptorHeap* m_rtvHeap = nullptr;
		DXBuffer* m_settingsBuffer = nullptr;

		void CreateRTVHeap();

		void CreatePipelineStateAndRootSignatureForStaticMesh();
		void CreatePipelineStateAndRootSignatureForSkeletalMesh();
	public:
		DXDeferredMaterial(const DXShader& vertexShader, const DXShader& vertexShaderForSkeletalMesh, const DXShader& pixelShader);
		virtual ~DXDeferredMaterial();

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