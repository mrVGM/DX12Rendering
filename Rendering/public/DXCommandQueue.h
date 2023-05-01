#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXCommandQueue : public BaseObject
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		bool Create(std::string& errorMessage);
	public:
		DXCommandQueue();

		ID3D12CommandQueue* GetCommandQueue();
	};
}