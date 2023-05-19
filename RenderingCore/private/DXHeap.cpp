#include "DXHeap.h"

#include "DXHeapMeta.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"

#include "JobSystem.h"

#include "BaseObjectContainer.h"

#include "DXResidentHeapFenceMeta.h"
#include "ResidentHeapJobSystemMeta.h"

#include "CoreUtils.h"

namespace
{
	int m_fenceProgress = 0;
	rendering::DXFence* m_residentHeapFence = nullptr;
	jobs::JobSystem* m_residentHeapJobSystem = nullptr;
	rendering::DXDevice* m_device = nullptr;

	void CacheObjects()
	{
		if (!m_device)
		{
			m_device = rendering::core::utils::GetDevice();
		}

		if (!m_residentHeapFence)
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();
			BaseObject* obj = container.GetObjectOfClass(rendering::DXResidentHeapFenceMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Resident Heap Fence!";
			}

			m_residentHeapFence = static_cast<rendering::DXFence*>(obj);
		}

		if (!m_residentHeapJobSystem)
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();
			BaseObject* obj = container.GetObjectOfClass(rendering::ResidentHeapJobSystemMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Resident Heap Job System!";
			}

			m_residentHeapJobSystem = static_cast<jobs::JobSystem*>(obj);
		}
	}
}

void rendering::DXHeap::MakeResident(jobs::Job* done)
{
	if (m_resident) {
		throw "The heap is already Resident!";
	}

	struct JobContext
	{
		DXHeap* m_heap = nullptr;
		jobs::Job* m_done = nullptr;
		int m_signal = 0;
	};

	class WaitJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		WaitJob(const JobContext& jobContext) :
			m_jobContext(jobContext)
		{
		}

		void Do() override
		{
			WaitFence waitFence(*m_residentHeapFence);
			waitFence.Wait(m_jobContext.m_signal);
			m_jobContext.m_heap->m_resident = true;

			core::utils::RunSync(m_jobContext.m_done);
		}
	};

	class EnqueJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		EnqueJob(const JobContext& jobContext) :
			m_jobContext(jobContext)
		{
		}

		void Do() override
		{
			ID3D12Device3* device3;
			HRESULT hr = m_device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
			if (FAILED(hr))
			{
				throw "Can't Query ID3D12Device3!";
			}
			const UINT64 signal = m_fenceProgress++;
			ID3D12Pageable* tmp = m_jobContext.m_heap->GetHeap();
			hr = device3->EnqueueMakeResident(D3D12_RESIDENCY_FLAGS::D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, &tmp, m_residentHeapFence->GetFence(), signal);
			if (FAILED(hr))
			{
				throw "Can't make the heap resident!";
			}

			m_jobContext.m_signal = signal;
			WaitJob* waitJob = new WaitJob(m_jobContext);
			core::utils::RunAsync(waitJob);
		}
	};

	JobContext jobContext;
	jobContext.m_heap = this;
	jobContext.m_done = done;

	m_residentHeapJobSystem->ScheduleJob(new EnqueJob(jobContext));
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
	CacheObjects();

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