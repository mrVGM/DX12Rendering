#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXPostProcessRP : public RenderPass
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		void Create();
		void RenderUnlit();

		void CreateQuadVertexBuffer(jobs::Job* done);
		void CreateQuadIndexBuffer(jobs::Job* done);

		void CreateMaterials(jobs::Job* done);

		void LoadBuffers(jobs::Job* done);
	public:
		DXPostProcessRP();
		virtual ~DXPostProcessRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}