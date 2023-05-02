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


		bool Create(std::string& errorMessage);
	public:
		DXClearRTRP();
		virtual ~DXClearRTRP();

		bool Prepare(std::string& errorMessage) override;
		bool Execute(std::string& errorMessage) override;
	};
}