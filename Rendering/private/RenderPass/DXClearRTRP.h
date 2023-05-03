#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXClearRTRP : public RenderPass
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;


		void Create();
	public:
		DXClearRTRP();
		virtual ~DXClearRTRP();

		void Prepare() override;
		void Execute() override;
	};
}