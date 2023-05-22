#include "RenderPass/DXDeferredRP.h"

#include "DXDeferredRPMeta.h"
#include "RenderUtils.h"

#include "DeferredRendering.h"
#include "ShaderRepo.h"

#include "DXDeferredMaterialMetaTag.h"

#include "Materials/DXShadowMapMaterial.h"
#include "Materials/DXShadowMapMaterialMeta.h"

#include "DXBufferMeta.h"
#include "DXHeap.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "BaseObjectContainer.h"

#include "DXLightsCalculationsMaterial.h"
#include "DXPostLightsCalculationsMaterial.h"

#include "Lights/LightsManager.h"
#include "Lights/LightsManagerMeta.h"

#include <set>
#include <list>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXMaterial* m_lightCalculationsMat = nullptr;
    rendering::DXMaterial* m_postLightCalculationsMat = nullptr;

    rendering::LightsManager* m_lightsManager = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        if (!m_lightsManager)
        {
            BaseObjectContainer& container = BaseObjectContainer::GetInstance();
            BaseObject* obj = container.GetObjectOfClass(LightsManagerMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Lights Manager!";
            }
            m_lightsManager = static_cast<LightsManager*>(obj);
        }
    }
}

rendering::DXDeferredRP::DXDeferredRP() :
    RenderPass(DXDeferredRPMeta::GetInstance())
{
    using Microsoft::WRL::ComPtr;

    new LightsManager();

    CacheObjects();

    DXDevice* device = utils::GetDevice();

    {
        THROW_ERROR(
            device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
            "Can't create Command Allocator!")

        THROW_ERROR(
            device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_startList)),
            "Can't reset Command List!")

        THROW_ERROR(
            m_startList->Close(),
            "Can't close Command List!")
    }
    
    {
        BaseObjectContainer& container = BaseObjectContainer::GetInstance();
        BaseObject* obj = container.GetObjectOfClass(DXShadowMapMaterialMeta::GetInstance());

        if (!obj)
        {
            obj = new DXShadowMapMaterial(*shader_repo::GetShadowMapVertexShader(), *shader_repo::GetShadowMapPixelShader());
        }

        m_shadowMapMaterial = static_cast<DXShadowMapMaterial*>(obj);
    }
}

rendering::DXDeferredRP::~DXDeferredRP()
{
    if (m_commandListsCache)
    {
        delete[] m_commandListsCache;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE rendering::DXDeferredRP::GetDescriptorHandleFor(GBufferTexType texType)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeap->GetDescriptorHandle(texType);
    return handle;
}

void rendering::DXDeferredRP::CreateRTVHeap()
{
    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferDiffuseTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularTex());
    textures.push_back(rendering::deferred::GetGBufferNormalTex());
    textures.push_back(rendering::deferred::GetGBufferPositionTex());

    m_rtvHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::CreateSRVHeap()
{
    LightsManager* lightsManager = m_lightsManager;

    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferDiffuseTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularTex());
    textures.push_back(rendering::deferred::GetGBufferNormalTex());
    textures.push_back(rendering::deferred::GetGBufferPositionTex());
    textures.push_back(lightsManager->GetShadowMap());

    m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::PrepareStartList()
{
    if (m_startListPrepared)
    {
        return;
    }

    DXDevice* device = utils::GetDevice();

    THROW_ERROR(
        m_startList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    LightsManager* lightsManager = m_lightsManager;
    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(lightsManager->GetShadowMap()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
        };
        m_startList->ResourceBarrier(_countof(barrier), barrier);
    }

    {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_startList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferTexType::Diffuse), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferTexType::Specular), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferTexType::Normal), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferTexType::Position), clearColor, 0, nullptr);
    }

    {
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = lightsManager->GetShadowMapDSDescriptorHeap()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        m_startList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        m_startList->ClearRenderTargetView(lightsManager->GetSMRTVHeap()->GetDescriptorHandle(0), clearColor, 0, nullptr);
    }



    THROW_ERROR(
        m_startList->Close(),
        "Can't close Command List!")

    m_startListPrepared = true;
}

void rendering::DXDeferredRP::RenderShadowMap()
{
    DXScene* scene = utils::GetScene();
    DXMaterialRepo* repo = utils::GetMaterialRepo();

    m_shadowMapMaterial->ResetCommandLists();

    std::list<ID3D12CommandList*> deferredLists;
    for (int i = 0; i < scene->m_scenesLoaded; ++i)
    {
        collada::ColladaScene& curColladaScene = *scene->m_colladaScenes[i];
        const DXScene::SceneResources& curSceneResources = scene->m_sceneResources[i];

        collada::Scene& s = curColladaScene.GetScene();

        for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
        {
            collada::Object& obj = it->second;
            collada::Geometry& geo = s.m_geometries[obj.m_geometry];
            int instanceIndex = s.m_objectInstanceMap[it->first];
            auto matOverrideIt = obj.m_materialOverrides.begin();

            for (auto it = geo.m_materials.begin(); it != geo.m_materials.end(); ++it)
            {
                DXMaterial* mat = repo->GetMaterial(*matOverrideIt);
                ++matOverrideIt;

                const DXScene::GeometryResources& geometryResources = curSceneResources.m_geometryResources.find(obj.m_geometry)->second;
                DXBuffer* vertBuf = geometryResources.m_vertexBuffer;
                DXBuffer* indexBuf = geometryResources.m_indexBuffer;
                DXBuffer* instanceBuf = geometryResources.m_instanceBuffer;

                if (!mat)
                {
                    continue;
                }

                if (!mat->GetMeta().HasTag(DXDeferredMaterialMetaTag::GetInstance()))
                {
                    continue;
                }

                deferredLists.push_back(m_shadowMapMaterial->GenerateCommandList(
                    *vertBuf,
                    *indexBuf,
                    *instanceBuf,
                    (*it).indexOffset,
                    (*it).indexCount,
                    instanceIndex));
            }
        }
    }

    int numLists = deferredLists.size();
    if (m_numCommandLists < numLists)
    {
        delete[] m_commandListsCache;
        m_commandListsCache = new ID3D12CommandList * [numLists];
        m_numCommandLists = numLists;
    }

    int index = 0;
    for (auto it = deferredLists.begin(); it != deferredLists.end(); ++it)
    {
        m_commandListsCache[index++] = *it;
    }

    DXCommandQueue* commandQueue = utils::GetCommandQueue();
    commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}

void rendering::DXDeferredRP::RenderDeferred()
{
    DXScene* scene = utils::GetScene();
    DXMaterialRepo* repo = utils::GetMaterialRepo();

    for (int i = 0; i < scene->m_scenesLoaded; ++i)
    {
        collada::ColladaScene& curColladaScene = *scene->m_colladaScenes[i];
        const DXScene::SceneResources& curSceneResources = scene->m_sceneResources[i];

        collada::Scene& s = curColladaScene.GetScene();

        for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
        {
            collada::Object& obj = it->second;
            for (auto it = obj.m_materialOverrides.begin(); it != obj.m_materialOverrides.end(); ++it)
            {
                DXMaterial* mat = repo->GetMaterial(*it);
                if (!mat)
                {
                    continue;
                }

                if (mat->GetMeta().HasTag(DXDeferredMaterialMetaTag::GetInstance()))
                {
                    mat->ResetCommandLists();
                }
            }
        }
    }

    std::list<ID3D12CommandList*> deferredLists;
    for (int i = 0; i < scene->m_scenesLoaded; ++i)
    {
        collada::ColladaScene& curColladaScene = *scene->m_colladaScenes[i];
        const DXScene::SceneResources& curSceneResources = scene->m_sceneResources[i];

        collada::Scene& s = curColladaScene.GetScene();

        for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
        {
            collada::Object& obj = it->second;
            collada::Geometry& geo = s.m_geometries[obj.m_geometry];
            int instanceIndex = s.m_objectInstanceMap[it->first];
            auto matOverrideIt = obj.m_materialOverrides.begin();

            for (auto it = geo.m_materials.begin(); it != geo.m_materials.end(); ++it)
            {
                DXMaterial* mat = repo->GetMaterial(*matOverrideIt);
                ++matOverrideIt;

                const DXScene::GeometryResources& geometryResources = curSceneResources.m_geometryResources.find(obj.m_geometry)->second;
                DXBuffer* vertBuf = geometryResources.m_vertexBuffer;
                DXBuffer* indexBuf = geometryResources.m_indexBuffer;
                DXBuffer* instanceBuf = geometryResources.m_instanceBuffer;

                if (!mat)
                {
                    continue;
                }

                if (!mat->GetMeta().HasTag(DXDeferredMaterialMetaTag::GetInstance()))
                {
                    continue;
                }

                deferredLists.push_back(mat->GenerateCommandList(
                    *vertBuf,
                    *indexBuf,
                    *instanceBuf,
                    (*it).indexOffset,
                    (*it).indexCount,
                    instanceIndex));
            }
        }
    }

    int numLists = deferredLists.size();
    if (m_numCommandLists < numLists)
    {
        delete[] m_commandListsCache;
        m_commandListsCache = new ID3D12CommandList * [numLists];
        m_numCommandLists = numLists;
    }

    int index = 0;
    for (auto it = deferredLists.begin(); it != deferredLists.end(); ++it)
    {
        m_commandListsCache[index++] = *it;
    }

    DXCommandQueue* commandQueue = utils::GetCommandQueue();
    commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}

void rendering::DXDeferredRP::Prepare()
{
    PrepareStartList();
}

void rendering::DXDeferredRP::Execute()
{
    DXCommandQueue* commandQueue = rendering::utils::GetCommandQueue();
    {
        ID3D12CommandList* ppCommandLists[] = { m_startList.Get() };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    RenderDeferred();
    RenderShadowMap();

    {
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_lightCalculationsMat->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(), 
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        m_postLightCalculationsMat->ResetCommandLists();
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_postLightCalculationsMat->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(),
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void rendering::DXDeferredRP::LoadLightsBuffer(jobs::Job* done)
{
    m_lightsManager->LoadLightsBuffer(done);
}

void rendering::DXDeferredRP::LoadShadowMap(jobs::Job* done)
{
    m_lightsManager->LoadShadowMap(done);
}


void rendering::DXDeferredRP::Load(jobs::Job* done)
{
    struct Context
    {
        DXDeferredRP* m_deferredRP = nullptr;
        int m_itemsLeft = 4;

        jobs::Job* m_done = nullptr;
    };

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
            --m_ctx.m_itemsLeft;
            if (m_ctx.m_itemsLeft > 0)
            {
                return;
            }
            
            m_ctx.m_deferredRP->CreateRTVHeap();
            m_ctx.m_deferredRP->CreateSRVHeap();


            m_lightCalculationsMat = new DXLightsCalculationsMaterial(
                *shader_repo::GetDeferredRPVertexShader(),
                *shader_repo::GetDeferredRPPixelShader());

            m_postLightCalculationsMat = new DXPostLightsCalculationsMaterial(
                *shader_repo::GetDeferredRPVertexShader(),
                *shader_repo::GetDeferredRPPostLightingPixelShader());

            utils::RunSync(m_ctx.m_done);

            delete& m_ctx;
        }
    };

    Context* ctx = new Context();
    ctx->m_deferredRP = this;
    ctx->m_done = done;

    deferred::LoadGBuffer(new ItemReady(*ctx));
    deferred::LoadGBufferLitTextures(new ItemReady(*ctx));

    LoadLightsBuffer(new ItemReady(*ctx));
    LoadShadowMap(new ItemReady(*ctx));
}


#undef THROW_ERROR