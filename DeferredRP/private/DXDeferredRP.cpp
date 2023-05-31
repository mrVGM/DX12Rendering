#include "DXDeferredRP.h"

#include "DXDeferredRPMeta.h"
#include "CoreUtils.h"

#include "DeferredRendering.h"
#include "ShaderRepo.h"

#include "DXDeferredMaterialMetaTag.h"

#include "HelperMaterials/DXShadowMapMaterial.h"
#include "HelperMaterials/DXShadowMapMaterialMeta.h"

#include "HelperMaterials/DXDisplaySMMaterial.h"

#include "DXBufferMeta.h"
#include "DXHeap.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "BaseObjectContainer.h"

#include "HelperMaterials/DXLightsCalculationsMaterial.h"
#include "HelperMaterials/DXPostLightsCalculationsMaterial.h"
#include "HelperMaterials/DXShadowMaskMaterial.h"

#include "LightsManager.h"
#include "LightsManagerMeta.h"

#include "DXTexture.h"

#include "DXScene.h"
#include "DXSceneMeta.h"

#include "DXMaterialRepo.h"
#include "DXMaterialRepoMeta.h"

#include "utils.h"

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
    rendering::DXMaterial* m_shadowMaskMat = nullptr;

    rendering::DXMaterial* m_displayTexMaterial = nullptr;

    rendering::LightsManager* m_lightsManager = nullptr;
    rendering::CascadedSM* m_cascadedSM = nullptr;

    rendering::DXDevice* m_device = nullptr;
    rendering::DXScene* m_scene = nullptr;
    rendering::DXMaterialRepo* m_materialRepo = nullptr;

    rendering::DXCommandQueue* m_commandQueue = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        if (!m_lightsManager)
        {
            m_lightsManager = deferred::GetLightsManager();
        }

        if (!m_cascadedSM)
        {
            m_cascadedSM = deferred::GetCascadedSM();
        }

        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }

        if (!m_commandQueue)
        {
            m_commandQueue = core::utils::GetCommandQueue();
        }

        if (!m_scene)
        {
            BaseObjectContainer& container = BaseObjectContainer::GetInstance();
            BaseObject* obj = container.GetObjectOfClass(DXSceneMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Scene!";
            }
            m_scene = static_cast<DXScene*>(obj);
        }

        if (!m_materialRepo)
        {
            BaseObjectContainer& container = BaseObjectContainer::GetInstance();
            BaseObject* obj = container.GetObjectOfClass(DXMaterialRepoMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Material Repo!";
            }
            m_materialRepo = static_cast<DXMaterialRepo*>(obj);
        }
    }
}

rendering::DXDeferredRP::DXDeferredRP() :
    RenderPass(DXDeferredRPMeta::GetInstance())
{
    using Microsoft::WRL::ComPtr;

    new LightsManager();
    new CascadedSM();

    CacheObjects();

    DXDevice* device = core::utils::GetDevice();

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

        THROW_ERROR(
            device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_afterRenderSceneList)),
            "Can't reset Command List!")

        THROW_ERROR(
            m_afterRenderSceneList->Close(),
            "Can't close Command List!")
    }
}

rendering::DXDeferredRP::~DXDeferredRP()
{
    if (m_commandListsCache)
    {
        delete[] m_commandListsCache;
    }
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
    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferDiffuseTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularTex());
    textures.push_back(rendering::deferred::GetGBufferNormalTex());
    textures.push_back(rendering::deferred::GetGBufferPositionTex());
    textures.push_back(m_cascadedSM->GetShadowMap());

    m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::PrepareStartList()
{
    if (m_startListPrepared)
    {
        return;
    }

    THROW_ERROR(
        m_startList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMap()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
        };
        m_startList->ResourceBarrier(_countof(barrier), barrier);
    }

    {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_startList->ClearRenderTargetView(m_rtvHeap->GetDescriptorHandle(0), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(m_rtvHeap->GetDescriptorHandle(1), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(m_rtvHeap->GetDescriptorHandle(2), clearColor, 0, nullptr);
        m_startList->ClearRenderTargetView(m_rtvHeap->GetDescriptorHandle(3), clearColor, 0, nullptr);

        m_startList->ClearRenderTargetView(m_cascadedSM->GetSMDescriptorHeap()->GetDescriptorHandle(0), clearColor, 0, nullptr);
    }

    {
        DXDescriptorHeap* dsDescriptorHeap = m_cascadedSM->GetDSDescriptorHeap();
        m_startList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        m_startList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(1), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        m_startList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(2), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        m_startList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(3), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    THROW_ERROR(
        m_startList->Close(),
        "Can't close Command List!")

    m_startListPrepared = true;
}

void rendering::DXDeferredRP::PrepareAfterRenderSceneList()
{
    if (m_afterRenderSceneListPrepared)
    {
        return;
    }

    THROW_ERROR(
        m_afterRenderSceneList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMap()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
        };
        m_afterRenderSceneList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        m_afterRenderSceneList->Close(),
        "Can't close Command List!")

    m_afterRenderSceneListPrepared = true;
}


void rendering::DXDeferredRP::RenderShadowMap()
{
    DXScene* scene = m_scene;
    DXMaterialRepo* repo = m_materialRepo;

    for (auto smMatIt = m_cascadedSM->GetShadowMapMaterials().begin();
        smMatIt != m_cascadedSM->GetShadowMapMaterials().end();
        ++smMatIt)
    {
        (*smMatIt)->ResetCommandLists();
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

                for (auto smMatIt = m_cascadedSM->GetShadowMapMaterials().begin();
                    smMatIt != m_cascadedSM->GetShadowMapMaterials().end();
                    ++smMatIt)
                {
                    deferredLists.push_back((*smMatIt)->GenerateCommandList(
                        *vertBuf,
                        *indexBuf,
                        *instanceBuf,
                        (*it).indexOffset,
                        (*it).indexCount,
                        instanceIndex));
                }
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

    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}

void rendering::DXDeferredRP::RenderDeferred()
{
    for (int i = 0; i < m_scene->m_scenesLoaded; ++i)
    {
        collada::ColladaScene& curColladaScene = *m_scene->m_colladaScenes[i];
        const DXScene::SceneResources& curSceneResources = m_scene->m_sceneResources[i];

        collada::Scene& s = curColladaScene.GetScene();

        for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
        {
            collada::Object& obj = it->second;
            for (auto it = obj.m_materialOverrides.begin(); it != obj.m_materialOverrides.end(); ++it)
            {
                DXMaterial* mat = m_materialRepo->GetMaterial(*it);
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
    for (int i = 0; i < m_scene->m_scenesLoaded; ++i)
    {
        collada::ColladaScene& curColladaScene = *m_scene->m_colladaScenes[i];
        const DXScene::SceneResources& curSceneResources = m_scene->m_sceneResources[i];

        collada::Scene& s = curColladaScene.GetScene();

        for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
        {
            collada::Object& obj = it->second;
            collada::Geometry& geo = s.m_geometries[obj.m_geometry];
            int instanceIndex = s.m_objectInstanceMap[it->first];
            auto matOverrideIt = obj.m_materialOverrides.begin();

            for (auto it = geo.m_materials.begin(); it != geo.m_materials.end(); ++it)
            {
                DXMaterial* mat = m_materialRepo->GetMaterial(*matOverrideIt);
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

    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}

void rendering::DXDeferredRP::Prepare()
{
    PrepareStartList();
    PrepareAfterRenderSceneList();
}

void rendering::DXDeferredRP::Execute()
{
    {
        ID3D12CommandList* ppCommandLists[] = { m_startList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    RenderDeferred();
    RenderShadowMap();

    {
        ID3D12CommandList* ppCommandLists[] = { m_afterRenderSceneList.Get() };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_shadowMaskMat->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(),
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_lightCalculationsMat->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(), 
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        m_postLightCalculationsMat->ResetCommandLists();
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_postLightCalculationsMat->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(),
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        m_displayTexMaterial->ResetCommandLists();
        DXBuffer* dummy = nullptr;
        ID3D12CommandList* commandList = m_displayTexMaterial->GenerateCommandList(
            *deferred::GetRenderTextureVertexBuffer(),
            *dummy, *dummy, 0, 0, 0);
        ID3D12CommandList* ppCommandLists[] = { commandList };
        m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void rendering::DXDeferredRP::LoadLightsBuffer(jobs::Job* done)
{
    m_lightsManager->LoadLightsBuffer(done);
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

            m_shadowMaskMat = new DXShadowMaskMaterial(
                *shader_repo::GetDeferredRPVertexShader(),
                *shader_repo::GetShadowMaskPixelShader());

            m_displayTexMaterial = new DXDisplaySMMaterial(
                *shader_repo::GetDeferredRPVertexShader(),
                *shader_repo::GetDisplayShadowMapPixelShader(),
                m_cascadedSM->GetShadowMap()
            );

            core::utils::RunSync(m_ctx.m_done);

            delete& m_ctx;
        }
    };

    Context* ctx = new Context();
    ctx->m_deferredRP = this;
    ctx->m_done = done;

    deferred::LoadGBuffer(new ItemReady(*ctx));
    deferred::LoadGBufferLitTextures(new ItemReady(*ctx));

    LoadLightsBuffer(new ItemReady(*ctx));

    m_cascadedSM->LoadResources(new ItemReady(*ctx));
}


#undef THROW_ERROR