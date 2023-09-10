#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXBuffer;

	class DXShadowMapMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineStateForSkeletalMesh;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignatureForSkeletalMesh;

		const DXShader& m_vertexShaderForSkeletalMesh;

		DXBuffer* m_materialSettingsBuffer = nullptr;

		int m_smSlot = 0;

		void CreatePipelineStateAndRootSignatureForStaticMesh();
		void CreatePipelineStateAndRootSignatureForSkeletalMesh();

	public:
		DXShadowMapMaterial(const DXShader& vertexShader, const DXShader& vertexShaderForSkeletalMesh, const DXShader& pixelShader, int smSlot);
		virtual ~DXShadowMapMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;

		ID3D12CommandList* GenerateCommandListForSkeletalMesh(
			const DXBuffer& vertexBuffer,
			const DXBuffer& jointsBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			const DXBuffer& skeletonBuffer,
			const DXBuffer& poseBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex);

		void LoadBuffer(jobs::Job* done);
	};
}