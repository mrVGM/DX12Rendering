#pragma once

#include "BaseObject.h"

#include "Job.h"

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
		bool Evict(std::string& errorMessage);
	public:

		DXHeap();
		virtual ~DXHeap();

		bool Create(std::string& errorMessage);
		bool MakeResident(std::string& errorMessage, jobs::Job* done);
		ID3D12Heap* GetHeap() const;
		void SetHeapSize(UINT64 size);
		void SetHeapType(D3D12_HEAP_TYPE type);
		void SetHeapFlags(D3D12_HEAP_FLAGS flags);

		const D3D12_HEAP_DESC& GetDescription() const;
	};
}