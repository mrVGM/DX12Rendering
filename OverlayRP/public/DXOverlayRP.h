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
		ID3D12CommandList** m_commandListsCache = nullptr;
		int m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		void Create();
		void RenderOverlay();
	public:
		DXOverlayRP();
		virtual ~DXOverlayRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}