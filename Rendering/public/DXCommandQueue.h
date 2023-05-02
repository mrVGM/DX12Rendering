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
		void Create();
	public:
		DXCommandQueue();
		virtual ~DXCommandQueue();

		ID3D12CommandQueue* GetCommandQueue();
	};
}