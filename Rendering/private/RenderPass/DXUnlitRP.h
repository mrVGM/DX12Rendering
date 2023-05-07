#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXUnlitRP : public RenderPass
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		void Create();
		void RenderUnlit();
	public:
		DXUnlitRP();
		virtual ~DXUnlitRP();

		void Prepare() override;
		void Execute() override;
	};
}