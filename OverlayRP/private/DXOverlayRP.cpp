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
#include "Resources/QuadInstanceBufferMeta.h"

#include "DXTexture.h"

#include "HelperMaterials/DXDisplayTextMaterial.h"

#include "ShaderRepo.h"

#include "Updaters/DXOverlayUpdater.h"

#include "ImageLoading.h"
#include "NotificationReceiver.h"
#include "Resources/FontLoadedNotificationReceiverMeta.h"

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

    rendering::DXBuffer* m_quadVertexBuffer = nullptr;
    rendering::DXBuffer* m_quadIndexBuffer = nullptr;
    rendering::DXMutableBuffer* m_quadInstanceBuffer = nullptr;

    rendering::DXMaterial* m_displayTextMaterial = nullptr;

    rendering::image_loading::FontAsset* m_fontAsset = nullptr;

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
            m_repo = overlay::GetMaterialRepo();
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
    RenderOverlay();
}

void rendering::overlay::DXOverlayRP::Load(jobs::Job* done)
{
    struct Context
    {
        DXOverlayRP* m_overlayRP = nullptr;
        jobs::Job* m_done = nullptr;
    };

    Context ctx {this, done};

    class BuffersReady : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        BuffersReady(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_overlayRP->CreateDisplayCharMaterial(m_ctx.m_done);
        }
    };

    LoadBuffers(new BuffersReady(ctx));
}

void rendering::overlay::DXOverlayRP::LoadBuffers(jobs::Job* done)
{
    struct Context
    {
        int m_left = 3;
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

            m_quadVertexBuffer = GetQuadVertexBuffer();
            m_quadIndexBuffer = GetQuadIndexBuffer();
            m_quadInstanceBuffer = GetQuadInstanceBuffer();

            const shader_repo::ShaderSet& shaderSet = shader_repo::GetShaderSetByName("display_char_mat");
            m_displayTextMaterial = new DXDisplayTextMaterial(*shaderSet.m_vertexShader, *shaderSet.m_pixelShader);

            new DXOverlayUpdater();

            delete &m_ctx;
        }
    };

    CreateQuadVertexBuffer(new ItemReady(*ctx));
    CreateQuadIndexBuffer(new ItemReady(*ctx));
    CreateQuadInstanceBuffer(new ItemReady(*ctx));
}

#undef THROW_ERROR

rendering::overlay::DXOverlayRP::DXOverlayRP() :
    RenderPass(DXOverlayRPMeta::GetInstance())
{
    CacheObjects();
    Create();

    m_textPanels.push_back(TextPanel{ "Hello World!", 100, 100 });
}

rendering::overlay::DXOverlayRP::~DXOverlayRP()
{
}


void rendering::overlay::DXOverlayRP::RenderOverlay()
{
    m_displayTextMaterial->ResetCommandLists();
    m_displayTextMaterial->GenerateCommandList(*m_quadVertexBuffer, *m_quadIndexBuffer, *m_quadInstanceBuffer->GetBuffer(), 0, 6, 0);

    const std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& lists = m_displayTextMaterial->GetGeneratedCommandLists();
    ID3D12CommandList* tmp = lists.front().Get();

    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(1, &tmp);
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
            QuadVertex* quadVerts = static_cast<QuadVertex*>(data);

            quadVerts[0] = QuadVertex{ {-1, -1, 0}, {0, 1} };
            quadVerts[1] = QuadVertex{ { 1, -1, 0}, {1, 1} };
            quadVerts[2] = QuadVertex{ { 1,  1, 0}, {1, 0} };
            quadVerts[3] = QuadVertex{ {-1,  1, 0}, {0, 0} };

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

void rendering::overlay::DXOverlayRP::CreateQuadInstanceBuffer(jobs::Job* done)
{
    DXMutableBuffer* instanceBuffer = new DXMutableBuffer(QuadInstanceBufferMeta::GetInstance(), m_maxCharacters * sizeof(CharInfo), sizeof(CharInfo));
    instanceBuffer->Load(done);
}

void rendering::overlay::DXOverlayRP::CreateDisplayCharMaterial(jobs::Job* done)
{
    if (!m_fontAsset)
    {
        m_fontAsset = GetFontAsset();
    }

    if (m_fontAsset)
    {
        const shader_repo::ShaderSet& shaderSet = shader_repo::GetShaderSetByName("display_char_mat");
        m_displayTextMaterial = new DXDisplayTextMaterial(*shaderSet.m_vertexShader, *shaderSet.m_pixelShader);
        
        core::utils::RunSync(done);
        return;
    }

    struct Context
    {
        jobs::Job* m_done = nullptr;
    };

    class FontLoadedReceiver : public notifications::NotificationReceiver
    {
    private:
        Context m_ctx;
    public:
        FontLoadedReceiver(const Context& ctx) :
            notifications::NotificationReceiver(FontLoadedNotificationReceiverMeta::GetInstance()),
            m_ctx(ctx)
        {
        }

        void Notify() override
        {
            m_fontAsset = GetFontAsset();

            if (!m_fontAsset)
            {
                return;
            }

            const shader_repo::ShaderSet& shaderSet = shader_repo::GetShaderSetByName("display_char_mat");
            m_displayTextMaterial = new DXDisplayTextMaterial(*shaderSet.m_vertexShader, *shaderSet.m_pixelShader);

            core::utils::RunSync(m_ctx.m_done);

            core::utils::DisposeBaseObject(*this);
        }
    };

    Context ctx{ done };
    new FontLoadedReceiver(ctx);
}

int rendering::overlay::DXOverlayRP::GetMaxCharacters() const
{
    return m_maxCharacters;
}

std::list<rendering::overlay::TextPanel>& rendering::overlay::DXOverlayRP::GetTextPannels()
{
    return m_textPanels;
}