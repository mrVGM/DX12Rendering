#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXCopyCommandQueue : public BaseObject
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		void Create();
	public:
		DXCopyCommandQueue();
		virtual ~DXCopyCommandQueue();

		ID3D12CommandQueue* GetCommandQueue();
	};
}