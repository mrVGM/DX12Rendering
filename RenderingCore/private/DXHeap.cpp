#include "DXHeap.h"

#include "DXHeapMeta.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"

#include "CoreUtils.h"


void rendering::DXHeap::MakeResident(jobs::Job* done)
{
	if (m_resident) {
		throw "The heap is already Resident!";
	}

	struct JobContext
	{
		DXHeap* m_heap = nullptr;
		DXFence* m_fence = nullptr;
		jobs::Job* m_done = nullptr;
		int m_signal = 0;
	};

	class WaitJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		WaitJob(JobContext jobContext) :
			m_jobContext(jobContext)
		{
		}

		void Do() override
		{
			WaitFence waitFence(*m_jobContext.m_fence);
			waitFence.Wait(m_jobContext.m_signal);
			m_jobContext.m_heap->m_resident = true;

			core::utils::RunSync(m_jobContext.m_done);
			core::utils::DisposeBaseObject(*m_jobContext.m_fence);
		}
	};

	class CreateFenceJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		CreateFenceJob(JobContext jobContext) :
			m_jobContext(jobContext)
		{
		}
		void Do() override
		{
			m_jobContext.m_fence = new DXFence(DXFenceMeta::GetInstance());

			DXDevice* device = core::utils::GetDevice();
			ID3D12Device3* device3;
			HRESULT hr = device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
			if (FAILED(hr))
			{
				throw "Can't Query ID3D12Device3!";
			}
			const UINT64 signal = 1;
			ID3D12Pageable* tmp = m_jobContext.m_heap->GetHeap();
			DXFence* fence = static_cast<DXFence*>(m_jobContext.m_fence);
			hr = device3->EnqueueMakeResident(D3D12_RESIDENCY_FLAGS::D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, &tmp, fence->GetFence(), signal);
			if (FAILED(hr))
			{
				throw "Can't make the heap resident!";
			}

			WaitJob* waitJob = new WaitJob(m_jobContext);
			core::utils::RunAsync(waitJob);
		}
	};

	JobContext jobContext;
	jobContext.m_heap = this;
	jobContext.m_done = done;

	core::utils::RunSync(new CreateFenceJob(jobContext));
}

void rendering::DXHeap::Evict()
{
	if (!m_resident) {
		throw "The heap is not Resident yet!";
	}

	DXDevice* device = core::utils::GetDevice();
	ID3D12Device3* device3;
	HRESULT hr = device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
	if (FAILED(hr)) {
		throw "Can't Query ID3D12Device3!";
	}

	ID3D12Pageable* const tmp = m_heap.Get();
	hr = device3->Evict(1, &tmp);
	if (FAILED(hr))
	{
		throw "Can't Evict the Heap!";
	}
	
	m_resident = false;
}

ID3D12Heap* rendering::DXHeap::GetHeap() const
{
	return m_heap.Get();
}

rendering::DXHeap::DXHeap() :
	BaseObject(DXHeapMeta::GetInstance())
{
	m_heapDescription.SizeInBytes = 256;
	m_heapDescription.Properties.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	m_heapDescription.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	m_heapDescription.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	m_heapDescription.Properties.CreationNodeMask = 0;
	m_heapDescription.Properties.VisibleNodeMask = 0;

	m_heapDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	m_heapDescription.Flags =
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_CREATE_NOT_ZEROED |
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
}

rendering::DXHeap::~DXHeap()
{
	if (m_resident) {
		Evict();
	}
}

void rendering::DXHeap::SetHeapSize(UINT64 size)
{
	m_heapDescription.SizeInBytes = size;
}

void rendering::DXHeap::SetHeapType(D3D12_HEAP_TYPE type)
{
	m_heapDescription.Properties.Type = type;
}

void rendering::DXHeap::SetHeapFlags(D3D12_HEAP_FLAGS flags)
{
	m_heapDescription.Flags = m_heapDescription.Flags | flags;
}

void rendering::DXHeap::Create()
{
	DXDevice* device = core::utils::GetDevice();
	HRESULT hr = device->GetDevice().CreateHeap(&m_heapDescription, IID_PPV_ARGS(&m_heap));
	if (FAILED(hr))
	{
		throw "Can't create Heap!";
	}
}

const D3D12_HEAP_DESC& rendering::DXHeap::GetDescription() const
{
	return m_heapDescription;
}