#pragma once

#include "BaseObject.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXClearRTCL : public BaseObject
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;


		bool Create(std::string& errorMessage);
	public:
		DXClearRTCL();
		virtual ~DXClearRTCL();

		bool Populate(std::string& errorMessage);
		bool Execute(std::string& errorMessage);
	};
}