#pragma once

#include "BaseObject.h"

#include "Job.h"
#include "JobSystem.h"

#include "d3dx12.h"

#include <wrl.h>
#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXHeap;
	class DXBuffer : public BaseObject
	{
		DXHeap* m_heap = nullptr;
		CD3DX12_RESOURCE_DESC m_bufferDescription = {};

		UINT64 m_size = -1;
		UINT64 m_stride = -1;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;

	public:
		ID3D12Resource* GetBuffer() const;
		UINT64 GetBufferSize() const;
		UINT64 GetStride() const;
		UINT64 GetElementCount() const;

		DXBuffer(const BaseObjectMeta& meta);
		virtual ~DXBuffer();

		void CopyData(void* data, int dataSize);
		void CopyBuffer(
			rendering::DXBuffer& destination,
			D3D12_RESOURCE_STATES myState,
			D3D12_RESOURCE_STATES destinationState,
			jobs::Job* done,
			jobs::JobSystem* jobSystem) const;

		void Place(DXHeap* heap, UINT64 heapOffset);

		void SetBufferSizeAndFlags(UINT64 size, D3D12_RESOURCE_FLAGS flags);
		void SetBufferStride(UINT64 size);

		void* Map();
		void Unmap();

		DXHeap* GetResidentHeap() const;
	};
}