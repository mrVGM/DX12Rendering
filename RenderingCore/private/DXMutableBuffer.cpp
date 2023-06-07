#include "DXMutableBuffer.h"

#include "DXBufferMeta.h"
#include "CoreUtils.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXHeap.h"

#include "ResourceUtils/DXCopyBuffers.h"
#include "ResourceUtils/DXCopyBuffersMeta.h"

rendering::DXMutableBuffer::DXMutableBuffer(const BaseObjectMeta& meta, UINT64 size, UINT64 stride, const D3D12_RESOURCE_FLAGS& flags) :
	BaseObject(meta)
{
	m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
	m_buffer->SetBufferSizeAndFlags(size, flags);
	m_buffer->SetBufferStride(stride);

	m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
	m_uploadBuffer->SetBufferSizeAndFlags(size, flags);
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
		DXMutableBuffer* m_self;
		DXHeap* m_heap = nullptr;
		DXHeap* m_uploadHeap = nullptr;

		int m_leftToLoad = 2;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_self = this;
	ctx->m_done = done;
	
	class ItemReady : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		ItemReady(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_leftToLoad;
			if (m_ctx.m_leftToLoad > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

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
			m_ctx.m_self->m_buffer->Place(m_ctx.m_heap, 0);
			core::utils::RunSync(new ItemReady(m_ctx));
		}
	};

	class UploadHeapLoaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		UploadHeapLoaded(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_self->m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);
			core::utils::RunSync(new ItemReady(m_ctx));
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
			m_ctx.m_heap->SetHeapSize(m_ctx.m_self->m_buffer->GetBufferSize());
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_heap->Create();

			m_ctx.m_uploadHeap = new DXHeap();
			m_ctx.m_uploadHeap->SetHeapSize(m_ctx.m_self->m_uploadBuffer->GetBufferSize());
			m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
			m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_uploadHeap->Create();

			m_ctx.m_heap->MakeResident(new HeapLoaded(m_ctx));
			m_ctx.m_uploadHeap->MakeResident(new UploadHeapLoaded(m_ctx));
		}
	};

	core::utils::RunSync(new CreateHeaps(*ctx));
}

void rendering::DXMutableBuffer::Copy(jobs::Job* done)
{
	m_uploadBuffer->CopyBuffer(*m_buffer, done);
}