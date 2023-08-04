#include "DXOverlayRP.h"

#include "DXOverlayRPMeta.h"
#include "CoreUtils.h"

#include "DXUnlitMaterialMetaTag.h"

#include "BaseObjectContainer.h"

#include "DXMaterial.h"
#include "DXMaterialRepo.h"
#include "DXMaterialRepoMeta.h"

#include "DXMutableBuffer.h"

#include "DXHeap.h"
#include "DXBufferMeta.h"
#include "Resources/QuadVertexBufferMeta.h"
#include "Resources/QuadIndexBufferMeta.h"

#include "utils.h"
#include "CoreUtils.h"

#include <set>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXCommandQueue* m_commandQueue = nullptr;
    rendering::DXMaterialRepo* m_repo = nullptr;

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

void rendering::overlay::DXOverlayRP::Create()
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

void rendering::overlay::DXOverlayRP::Prepare()
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

void rendering::overlay::DXOverlayRP::Execute()
{
    {
        ID3D12CommandList* ppCommandLists[] = { m_startList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        ID3D12CommandList* ppCommandLists[] = { m_endList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void rendering::overlay::DXOverlayRP::Load(jobs::Job* done)
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
            delete &m_ctx;
        }
    };

    CreateQuadVertexBuffer(new ItemReady(*ctx));
    CreateQuadIndexBuffer(new ItemReady(*ctx));
}

#undef THROW_ERROR

rendering::overlay::DXOverlayRP::DXOverlayRP() :
    RenderPass(DXOverlayRPMeta::GetInstance())
{
    CacheObjects();
    Create();
}

rendering::overlay::DXOverlayRP::~DXOverlayRP()
{
    if (m_commandListsCache)
    {
        delete[] m_commandListsCache;
    }
}


void rendering::overlay::DXOverlayRP::RenderOverlay()
{
    DXMaterial* errorMat = m_repo->GetMaterial("error");
    if (errorMat)
    {
        errorMat->ResetCommandLists();
    }

    std::list<ID3D12CommandList*> unlitLists;

    int numLists = unlitLists.size();
    if (m_numCommandLists < numLists)
    {
        delete[] m_commandListsCache;
        m_commandListsCache = new ID3D12CommandList* [numLists];
        m_numCommandLists = numLists;
    }

    int index = 0;
    for (auto it = unlitLists.begin(); it != unlitLists.end(); ++it)
    {
        m_commandListsCache[index++] = *it;
    }

    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}

void rendering::overlay::DXOverlayRP::CreateQuadIndexBuffer(jobs::Job* done)
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
            QuadVertex* quadVerts = static_cast<QuadVertex*>(data);

            quadVerts[0] = QuadVertex{ {-1, -1, 0}, {0, 0} };
            quadVerts[1] = QuadVertex{ { 1, -1, 0}, {1, 0} };
            quadVerts[2] = QuadVertex{ { 1,  1, 0}, {1, 1} };
            quadVerts[3] = QuadVertex{ {-1,  1, 0}, {0, 1} };

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
            UINT64 stride = sizeof(QuadVertex);
            UINT64 size = 4 * stride;

            m_ctx.m_buffer = new DXBuffer(QuadVertexBufferMeta::GetInstance());
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

void rendering::overlay::DXOverlayRP::CreateQuadVertexBuffer(jobs::Job* done)
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

            m_ctx.m_buffer = new DXBuffer(QuadIndexBufferMeta::GetInstance());
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