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
		std::string m_heapType;
		std::string m_heapBufferType;

		ID3D12Device3* m_device3 = nullptr;
		bool m_resident = false;

		Microsoft::WRL::ComPtr<ID3D12Heap> m_heap;
		bool Init(const D3D12_HEAP_DESC& desc, std::string errorMessage);
		bool MakeResident(std::string& errorMessage, jobs::Job* done);
		bool Evict(std::string& errorMessage);
		bool Create(std::string& errorMessage);
	public:
		DXHeap();
		virtual ~DXHeap();

		ID3D12Heap* GetHeap() const;
		void SetHeapSize(UINT64 size);
		void SetHeapType(D3D12_HEAP_TYPE type);
		void SetHeapFlags(D3D12_HEAP_FLAGS flags);
	};
}