#include "DXBuffer.h"

#include "DXBufferMeta.h"
#include "RenderUtils.h"

#include "DXHeap.h"

#include "ResourceUtils/DXCopyBuffers.h"
#include "TemporaryBaseObject.h"

#include <list>


void rendering::DXBuffer::SetBufferSizeAndFlags(UINT64 size, D3D12_RESOURCE_FLAGS flags)
{
	m_size = size;
	m_bufferDescription = CD3DX12_RESOURCE_DESC::Buffer(m_size, flags);
}

void rendering::DXBuffer::SetBufferStride(UINT64 stride)
{
	m_stride = stride;
}

rendering::DXBuffer::DXBuffer(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
}

rendering::DXBuffer::~DXBuffer()
{
}

void rendering::DXBuffer::CopyData(void* data, int dataSize)
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		throw "Can't map Vertex Buffer!";
	}

	memcpy(dst, data, dataSize);
	m_buffer->Unmap(0, nullptr);
}


void rendering::DXBuffer::Place(DXHeap* heap, UINT64 heapOffset)
{
	DXDevice* device = rendering::utils::GetDevice();
	D3D12_HEAP_TYPE heapType = heap->GetDescription().Properties.Type;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
	}

	HRESULT hRes = device->GetDevice().CreatePlacedResource(heap->GetHeap(), heapOffset, &m_bufferDescription, initialState, nullptr, IID_PPV_ARGS(&m_buffer));
	if (FAILED(hRes))
	{
		throw "Can't place buffer in the heap!";
	}

	m_heap = heap;
}

ID3D12Resource* rendering::DXBuffer::GetBuffer() const
{
	return m_buffer.Get();
}

UINT64 rendering::DXBuffer::GetBufferSize() const
{
	return m_size;
}

UINT64 rendering::DXBuffer::GetStride() const
{
	return m_stride;
}

UINT64 rendering::DXBuffer::GetElementCount() const
{
	return m_size / m_stride;
}


void rendering::DXBuffer::CopyBuffer(
	rendering::DXBuffer& destination,
	jobs::Job* done,
	jobs::JobSystem* jobSystem) const
{
	struct Context
	{
		const DXBuffer* src = nullptr;
		DXBuffer* dst = nullptr;
		DXCopyBuffers* m_copyBuffers = nullptr;

		jobs::Job* m_done;
		jobs::JobSystem* m_jobSystem;
	};

	Context ctx;
	ctx.src = this;
	ctx.dst = &destination;
	ctx.m_done = done;
	ctx.m_jobSystem = jobSystem;

	class Clear : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		Clear(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			delete m_ctx.m_copyBuffers;
			m_ctx.m_jobSystem->ScheduleJob(m_ctx.m_done);
		}
	};

	class CopyBuffers : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CopyBuffers(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_copyBuffers->Execute(*m_ctx.dst, *m_ctx.src, new Clear(m_ctx), utils::GetMainJobSystem());
		}
	};

	class CreateCopyBuffers : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CreateCopyBuffers(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_copyBuffers = new DXCopyBuffers();
			utils::GetLoadJobSystem()->ScheduleJob(new CopyBuffers(m_ctx));
		}
	};

	utils::GetMainJobSystem()->ScheduleJob(new CreateCopyBuffers(ctx));
}

void* rendering::DXBuffer::Map()
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		throw "Can't map Vertex Buffer!";
	}

	return dst;
}

void rendering::DXBuffer::Unmap()
{
	m_buffer->Unmap(0, nullptr);
}

rendering::DXHeap* rendering::DXBuffer::GetResidentHeap() const
{
	return m_heap;
}