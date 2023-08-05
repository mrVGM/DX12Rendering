#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering::overlay
{
	class DXOverlayRP : public RenderPass
	{
		const int m_maxCharacters = 1024;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		void Create();
		void RenderOverlay();

		void CreateQuadVertexBuffer(jobs::Job* done);
		void CreateQuadIndexBuffer(jobs::Job* done);
		void CreateQuadInstanceBuffer(jobs::Job* done);
	public:
		DXOverlayRP();
		virtual ~DXOverlayRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}