#pragma once

#include "BaseObject.h"

#include "Job.h"
#include "JobSystem.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXHeap : public BaseObject
	{
		D3D12_HEAP_DESC m_heapDescription = {};
		bool m_resident = false;

		Microsoft::WRL::ComPtr<ID3D12Heap> m_heap;
		void Evict();
	public:

		DXHeap();
		virtual ~DXHeap();

		void Create();
		void MakeResident(jobs::Job* done, jobs::JobSystem* jobSystem);
		ID3D12Heap* GetHeap() const;
		void SetHeapSize(UINT64 size);
		void SetHeapType(D3D12_HEAP_TYPE type);
		void SetHeapFlags(D3D12_HEAP_FLAGS flags);

		const D3D12_HEAP_DESC& GetDescription() const;
	};
}