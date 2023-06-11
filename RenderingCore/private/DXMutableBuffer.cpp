#include "DXMutableBuffer.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXHeap.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::DXMutableBuffer::DXMutableBuffer(const BaseObjectMeta& meta, UINT64 size, UINT64 stride) :
	BaseObject(meta)
{
	using Microsoft::WRL::ComPtr;

	m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
	m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
	m_buffer->SetBufferStride(stride);

	m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
	m_uploadBuffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
	m_uploadBuffer->SetBufferStride(stride);
}

rendering::DXMutableBuffer::~DXMutableBuffer()
{
}

rendering::DXBuffer* rendering::DXMutableBuffer::GetBuffer()
{
	return m_buffer;
}

rendering::DXBuffer* rendering::DXMutableBuffer::GetUploadBuffer()
{
	return m_uploadBuffer;
}

void rendering::DXMutableBuffer::Load(jobs::Job* done)
{
	struct Context
	{
		DXMutableBuffer* m_mutableBuffer = nullptr;

		DXHeap* m_heap = nullptr;
		DXHeap* m_uploadHeap = nullptr;

		int m_loadsToWaitFor = 2;

		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_mutableBuffer = this;
	ctx->m_done = done;

	class HeapLoaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		HeapLoaded(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_loadsToWaitFor;
			if (m_ctx.m_loadsToWaitFor > 0)
			{
				return;
			}

			m_ctx.m_mutableBuffer->GetBuffer()->Place(m_ctx.m_heap, 0);
			m_ctx.m_mutableBuffer->GetUploadBuffer()->Place(m_ctx.m_uploadHeap, 0);

			m_ctx.m_mutableBuffer->CreateCommandList();

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	class CreateHeaps : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		CreateHeaps(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_mutableBuffer->GetBuffer()->GetBufferSize());
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_heap->Create();


			m_ctx.m_uploadHeap = new DXHeap();
			m_ctx.m_uploadHeap->SetHeapSize(m_ctx.m_mutableBuffer->GetBuffer()->GetBufferSize());
			m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
			m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_uploadHeap->Create();

			m_ctx.m_heap->MakeResident(new HeapLoaded(m_ctx));
			m_ctx.m_uploadHeap->MakeResident(new HeapLoaded(m_ctx));
		}
	};

	core::utils::RunSync(new CreateHeaps(*ctx));
}

void rendering::DXMutableBuffer::Upload(jobs::Job* done)
{
	m_uploadBuffer->CopyBuffer(*m_buffer, done);
}

void rendering::DXMutableBuffer::CreateCommandList()
{
	DXDevice* device = core::utils::GetDevice();

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
		"Can't create Command List!")

	m_commandList->CopyResource(m_buffer->GetBuffer(), m_uploadBuffer->GetBuffer());

	THROW_ERROR(
		m_commandList->Close(),
		"Can't close command List!")
}

#undef THROW_ERROR