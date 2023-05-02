#include "DXHeap.h"

#include "DXHeapMeta.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"

#include "RenderUtils.h"
#include "TemporaryBaseObject.h"
#include "Job.h"

bool rendering::DXHeap::MakeResident(std::string& errorMessage, jobs::Job* done)
{
	if (m_resident) {
		errorMessage = "The heap is already Resident!";
		return false;
	}

	struct JobContext
	{
		DXHeap* m_heap = nullptr;
		TemporaryBaseObject* m_fenceTempObject = nullptr;
		jobs::Job* m_done = nullptr;
	};

	class MakeResidentJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		MakeResidentJob(JobContext jobContext) :
			m_jobContext(jobContext)
		{
		}
		void Do() override
		{
			DXDevice* device = rendering::utils::GetDevice();
			ID3D12Device3* device3;
			HRESULT hr = device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
			if (FAILED(hr))
			{
				throw "Can't Query ID3D12Device3!";
			}
			const UINT64 signal = 1;
			ID3D12Pageable* tmp = m_jobContext.m_heap->GetHeap();
			DXFence* fence = static_cast<DXFence*>(m_jobContext.m_fenceTempObject->m_object);
			hr = device3->EnqueueMakeResident(D3D12_RESIDENCY_FLAGS::D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, &tmp, fence->GetFence(), signal);
			if (FAILED(hr))
			{
				throw "Can't make the heap resident!";
			}

			WaitFence waitFence(*fence);
			std::string error;
			bool res = waitFence.Wait(signal, error);

			if (!res)
			{
				throw error;
			}
			m_jobContext.m_heap->m_resident = true;

			delete m_jobContext.m_fenceTempObject;
			
			jobs::JobSystem* loadJobSystem = rendering::utils::GetLoadJobSystem();
			loadJobSystem->ScheduleJob(m_jobContext.m_done);
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
			m_jobContext.m_fenceTempObject->m_object = new DXFence(DXFenceMeta::GetInstance());

			MakeResidentJob* makeResident = new MakeResidentJob(m_jobContext);
			jobs::JobSystem* loadJobSystem = rendering::utils::GetLoadJobSystem();
			loadJobSystem->ScheduleJob(makeResident);
		}
	};

	JobContext jobContext;
	jobContext.m_heap = this;
	jobContext.m_done = done;
	jobContext.m_fenceTempObject = new TemporaryBaseObject();

	jobContext.m_fenceTempObject->CreateObject(new CreateFenceJob(jobContext));

	return true;
}

bool rendering::DXHeap::Evict(std::string& errorMessage)
{
	if (!m_resident) {
		errorMessage = "The heap is not Resident yet!";
		return false;
	}

	DXDevice* device = rendering::utils::GetDevice();
	ID3D12Device3* device3;
	HRESULT hr = device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
	if (FAILED(hr)) {
		errorMessage = "Can't Query ID3D12Device3!";
		return false;
	}

	ID3D12Pageable* const tmp = m_heap.Get();
	hr = device3->Evict(1, &tmp);
	if (FAILED(hr))
	{
		errorMessage = "Can't Evict the Heap!";
		return false;
	}
	
	m_resident = false;

	return true;
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
		std::string error;
		bool res = Evict(error);
		if (!res)
		{
			throw error;
		}
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

bool rendering::DXHeap::Create(std::string& errorMessage)
{
	DXDevice* device = rendering::utils::GetDevice();
	HRESULT hr = device->GetDevice().CreateHeap(&m_heapDescription, IID_PPV_ARGS(&m_heap));
	if (FAILED(hr))
	{
		errorMessage = "Can't create Heap!";
		return false;
	}

	return true;
}

const D3D12_HEAP_DESC& rendering::DXHeap::GetDescription() const
{
	return m_heapDescription;
}