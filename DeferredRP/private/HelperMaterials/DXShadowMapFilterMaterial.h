#pragma once

#include "DXMaterial.h"

#include "Job.h"

namespace rendering
{
	class DXDescriptorHeap;
	class DXTexture;

	class DXShadowMapFilterMaterial : public rendering::DXMaterial
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		DXTexture* m_srcTex = nullptr;
		DXTexture* m_dstTex = nullptr;

		DXDescriptorHeap* m_srvHeap = nullptr;
		DXDescriptorHeap* m_rtvHeap = nullptr;

		void CreatePipelineStateAndRootSignature();
		void CreateDescriptorHeaps();

	public:
		DXShadowMapFilterMaterial(
			const DXShader& vertexShader,
			const DXShader& pixelShader,
			DXTexture* srcTex,
			DXTexture* dstTex
		);
		virtual ~DXShadowMapFilterMaterial();

		virtual ID3D12CommandList* GenerateCommandList(
			const DXBuffer& vertexBuffer,
			const DXBuffer& indexBuffer,
			const DXBuffer& instanceBuffer,
			UINT startIndex,
			UINT indexCount,
			UINT instanceIndex) override;
	};
}