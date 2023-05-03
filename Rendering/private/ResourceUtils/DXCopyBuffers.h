#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXBuffer;
	class DXCopyBuffers : public BaseObject
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		void Create();
	public:
		DXCopyBuffers();
		virtual ~DXCopyBuffers();

		void Execute(
			DXBuffer& dst,
			D3D12_RESOURCE_STATES dstInitialState,
			const DXBuffer& src,
			D3D12_RESOURCE_STATES srcInitialState,
			jobs::Job* done
		);
	};
}