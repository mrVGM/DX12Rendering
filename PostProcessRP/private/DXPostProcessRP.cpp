#include "DXPostProcessRP.h"

#include "DXPostProcessRPMeta.h"

#include "DXUnlitMaterialMetaTag.h"


#include "BaseObjectContainer.h"

#include "DXMaterial.h"
#include "DXMaterialRepo.h"
#include "DXMaterialRepoMeta.h"

#include "DXHeap.h"

#include "Resources/CanvasVertexBufferMeta.h"
#include "Resources/CanvasIndexBufferMeta.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "CoreUtils.h"
#include "utils.h"

#include <set>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    struct CanvasVertex
    {
        float m_position[3];
        float m_uv[2];
    };

    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXCommandQueue* m_commandQueue = nullptr;
    rendering::DXMaterialRepo* m_repo = nullptr;

    rendering::DXBuffer* m_canvasVertexBuffer = nullptr;
    rendering::DXBuffer* m_canvasIndexBuffer = nullptr;

    void CacheObjects()
    {
        using namespace rendering;

        if (!m_swapChain)
        {
            m_swapChain = core::utils::GetSwapChain();
        }

        if (!m_commandQueue)
        {
            m_commandQueue = core::utils::GetCommandQueue();
        }

        if (!m_repo)
        {
            BaseObjectContainer& container = BaseObjectContainer::GetInstance();
            BaseObject* obj = container.GetObjectOfClass(DXMaterialRepoMeta::GetInstance());

            if (!obj)
            {
                throw "Can't Find Material Repo!";
            }

            m_repo = static_cast<DXMaterialRepo*>(obj);
        }
    }
}

void rendering::DXPostProcessRP::Create()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = core::utils::GetDevice();
    if (!device)
    {
        throw "No device found!";
    }

    THROW_ERROR(
        device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
        "Can't create Command Allocator!")

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_startList)),
        "Can't create Command List!")

    THROW_ERROR(
        m_startList->Close(),
        "Can't close command List!")

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_endList)),
        "Can't create Command List!")

    THROW_ERROR(
        m_endList->Close(),
        "Can't close command List!")
}

void rendering::DXPostProcessRP::Prepare()
{
    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_startList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_startList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_startList->Close(),
        "Can't close command List!")

    THROW_ERROR(
        m_endList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_endList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_endList->Close(),
        "Can't close Command List!")



}

void rendering::DXPostProcessRP::Execute()
{
    {
        ID3D12CommandList* ppCommandLists[] = { m_startList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    RenderUnlit();

    {
        ID3D12CommandList* ppCommandLists[] = { m_endList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void rendering::DXPostProcessRP::Load(jobs::Job* done)
{
    core::utils::RunSync(done);
}


rendering::DXPostProcessRP::DXPostProcessRP() :
    RenderPass(DXPostProcessRPMeta::GetInstance())
{
    CacheObjects();
    Create();
}

rendering::DXPostProcessRP::~DXPostProcessRP()
{
}


void rendering::DXPostProcessRP::RenderUnlit()
{
}


void rendering::DXPostProcessRP::CreateQuadVertexBuffer(jobs::Job* done)
{
    struct Context
    {
        int m_loading = 2;

        DXBuffer* m_buffer = nullptr;
        DXHeap* m_heap = nullptr;

        DXBuffer* m_uploadBuffer = nullptr;
        DXHeap* m_uploadHeap = nullptr;

        jobs::Job* m_done = nullptr;
    };

    Context* ctx = new Context();
    ctx->m_done = done;

    class CleanUp : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        CleanUp(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            core::utils::RunSync(m_ctx.m_done);

            core::utils::DisposeBaseObject(*m_ctx.m_uploadBuffer);
            core::utils::DisposeBaseObject(*m_ctx.m_uploadHeap);
            delete& m_ctx;
        }
    };

    class PlaceBuffers : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        PlaceBuffers(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
            m_ctx.m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);

            void* data = m_ctx.m_uploadBuffer->Map();
            CanvasVertex* canvasVerts = static_cast<CanvasVertex*>(data);

            canvasVerts[0] = CanvasVertex{ {-1, -1, 0}, {0, 1} };
            canvasVerts[1] = CanvasVertex{ { 1, -1, 0}, {1, 1} };
            canvasVerts[2] = CanvasVertex{ { 1,  1, 0}, {1, 0} };
            canvasVerts[3] = CanvasVertex{ {-1,  1, 0}, {0, 0} };

            m_ctx.m_uploadBuffer->Unmap();

            m_ctx.m_uploadBuffer->CopyBuffer(*m_ctx.m_buffer, new CleanUp(m_ctx));
        }
    };

    class HeapIsResident : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        HeapIsResident(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            --m_ctx.m_loading;
            if (m_ctx.m_loading > 0)
            {
                return;
            }

            core::utils::RunAsync(new PlaceBuffers(m_ctx));
        }
    };

    class CreateItems : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        CreateItems(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            UINT64 stride = sizeof(CanvasVertex);
            UINT64 size = 4 * stride;

            m_ctx.m_buffer = new DXBuffer(CanvasVertexBufferMeta::GetInstance());
            m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_buffer->SetBufferStride(stride);

            m_ctx.m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
            m_ctx.m_uploadBuffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_uploadBuffer->SetBufferStride(stride);

            m_ctx.m_heap = new DXHeap();
            m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
            m_ctx.m_heap->SetHeapSize(size);
            m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
            m_ctx.m_heap->Create();

            m_ctx.m_uploadHeap = new DXHeap();
            m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
            m_ctx.m_uploadHeap->SetHeapSize(size);
            m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
            m_ctx.m_uploadHeap->Create();

            m_ctx.m_heap->MakeResident(new HeapIsResident(m_ctx));
            m_ctx.m_uploadHeap->MakeResident(new HeapIsResident(m_ctx));
        }
    };

    core::utils::RunSync(new CreateItems(*ctx));
}

void rendering::DXPostProcessRP::CreateQuadIndexBuffer(jobs::Job* done)
{
    struct Context
    {
        int m_loading = 2;

        DXBuffer* m_buffer = nullptr;
        DXHeap* m_heap = nullptr;

        DXBuffer* m_uploadBuffer = nullptr;
        DXHeap* m_uploadHeap = nullptr;

        jobs::Job* m_done = nullptr;
    };

    Context* ctx = new Context();
    ctx->m_done = done;

    class CleanUp : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        CleanUp(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            core::utils::RunSync(m_ctx.m_done);

            core::utils::DisposeBaseObject(*m_ctx.m_uploadBuffer);
            core::utils::DisposeBaseObject(*m_ctx.m_uploadHeap);
            delete& m_ctx;
        }
    };

    class PlaceBuffers : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        PlaceBuffers(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
            m_ctx.m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);

            void* data = m_ctx.m_uploadBuffer->Map();
            int* indices = static_cast<int*>(data);

            indices[0] = 0;
            indices[1] = 3;
            indices[2] = 2;
            indices[3] = 0;
            indices[4] = 2;
            indices[5] = 1;

            m_ctx.m_uploadBuffer->Unmap();

            m_ctx.m_uploadBuffer->CopyBuffer(*m_ctx.m_buffer, new CleanUp(m_ctx));
        }
    };

    class HeapIsResident : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        HeapIsResident(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            --m_ctx.m_loading;
            if (m_ctx.m_loading > 0)
            {
                return;
            }

            core::utils::RunAsync(new PlaceBuffers(m_ctx));
        }
    };

    class CreateItems : public jobs::Job
    {
    private:
        Context& m_ctx;
    public:
        CreateItems(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            UINT64 stride = sizeof(int);
            UINT64 size = 6 * stride;

            m_ctx.m_buffer = new DXBuffer(CanvasIndexBufferMeta::GetInstance());
            m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_buffer->SetBufferStride(stride);

            m_ctx.m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
            m_ctx.m_uploadBuffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_uploadBuffer->SetBufferStride(stride);

            m_ctx.m_heap = new DXHeap();
            m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
            m_ctx.m_heap->SetHeapSize(size);
            m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
            m_ctx.m_heap->Create();

            m_ctx.m_uploadHeap = new DXHeap();
            m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
            m_ctx.m_uploadHeap->SetHeapSize(size);
            m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
            m_ctx.m_uploadHeap->Create();

            m_ctx.m_heap->MakeResident(new HeapIsResident(m_ctx));
            m_ctx.m_uploadHeap->MakeResident(new HeapIsResident(m_ctx));
        }
    };

    core::utils::RunSync(new CreateItems(*ctx));
}

void rendering::DXPostProcessRP::CreateMaterials(jobs::Job* done)
{
}

void rendering::DXPostProcessRP::LoadBuffers(jobs::Job* done)
{
    struct Context
    {
        int m_left = 2;
        jobs::Job* m_done = nullptr;
    };

    Context* ctx = new Context();
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
            --m_ctx.m_left;
            if (m_ctx.m_left > 0)
            {
                return;
            }

            core::utils::RunSync(m_ctx.m_done);

            m_canvasVertexBuffer = GetCanvasVertexBuffer();
            m_canvasIndexBuffer = GetCanvasIndexBuffer();

            delete& m_ctx;
        }
    };

    CreateQuadVertexBuffer(new ItemReady(*ctx));
    CreateQuadIndexBuffer(new ItemReady(*ctx));
}


#undef THROW_ERROR