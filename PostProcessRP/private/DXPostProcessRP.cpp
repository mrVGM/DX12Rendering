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

#include "ShaderRepo.h"

#include "Materials/DXOutlineMaterial.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "TextureLoadedNotificationMeta.h"
#include "NotificationReceiver.h"

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

    rendering::DXMaterial* m_outlineMat = nullptr;

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

    bool CheckRequiredTextures()
    {
        using namespace rendering;

        {
            DXTexture* tex = GetCameraDepthTetxure();
            if (!tex || !tex->IsLoaded())
            {
                return false;
            }
        }

        {
            DXTexture* tex = GetNormalsTetxure();
            if (!tex || !tex->IsLoaded())
            {
                return false;
            }
        }

        {
            DXTexture* tex = GetPositionTetxure();
            if (!tex || !tex->IsLoaded())
            {
                return false;
            }
        }

        return true;
    }
}

void rendering::DXPostProcessRP::Create()
{
}

void rendering::DXPostProcessRP::Prepare()
{
}

void rendering::DXPostProcessRP::Execute()
{
    RenderPP();
}

void rendering::DXPostProcessRP::Load(jobs::Job* done)
{
    struct Context
    {
        DXPostProcessRP* m_postProcessRP = nullptr;

        jobs::Job* m_done = nullptr;
    };

    Context ctx{ this, done };

    class BuffersLoaded : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        BuffersLoaded(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_postProcessRP->CreateMaterials(m_ctx.m_done);
        }
    };

    LoadBuffers(new BuffersLoaded(ctx));
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


void rendering::DXPostProcessRP::RenderPP()
{
    DXBuffer* dummy = nullptr;
    ID3D12CommandList* lists[] = { m_outlineMat->GenerateCommandList(*m_canvasVertexBuffer, *m_canvasIndexBuffer, *dummy, 0, 6, 0) };

    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(lists), lists);
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
    if (CheckRequiredTextures())
    {
        const shader_repo::ShaderSet& outlineMaterialSet = shader_repo::GetShaderSetByName("pp_outline_mat");
        DXOutlineMaterial* mat = new DXOutlineMaterial(*outlineMaterialSet.m_vertexShader, *outlineMaterialSet.m_pixelShader);
        m_outlineMat = mat;
        mat->LoadSettingsBuffer(done);
        return;
    }

    class WaitForCameraDepthTex : public notifications::NotificationReceiver
    {
    private:
        jobs::Job* m_done = nullptr;
    public:
        WaitForCameraDepthTex(jobs::Job* done) :
            NotificationReceiver(TextureLoadedNotificationMeta::GetInstance()),
            m_done(done)
        {
        }

        void Notify() override
        {
            if (!CheckRequiredTextures())
            {
                return;
            }

            const shader_repo::ShaderSet& outlineMaterialSet = shader_repo::GetShaderSetByName("pp_outline_mat");
            DXOutlineMaterial* mat = new DXOutlineMaterial(*outlineMaterialSet.m_vertexShader, *outlineMaterialSet.m_pixelShader);
            m_outlineMat = mat;
            mat->LoadSettingsBuffer(m_done);

            delete this;
        }
    };

    new WaitForCameraDepthTex(done);
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