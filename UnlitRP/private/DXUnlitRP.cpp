#include "DXUnlitRP.h"

#include "DXUnlitRPMeta.h"
#include "CoreUtils.h"

#include "DXUnlitMaterialMetaTag.h"

#include "DXScene.h"
#include "DXSceneMeta.h"

#include "BaseObjectContainer.h"

#include "DXMaterial.h"
#include "DXMaterialRepo.h"
#include "DXMaterialRepoMeta.h"

#include <set>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXCommandQueue* m_commandQueue = nullptr;
    rendering::DXScene* m_scene = nullptr;
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

        if (!m_scene)
        {
            BaseObjectContainer& container = BaseObjectContainer::GetInstance();
            BaseObject* obj = container.GetObjectOfClass(DXSceneMeta::GetInstance());

            if (!obj)
            {
                throw "Can't Find Scene!";
            }

            m_scene = static_cast<DXScene*>(obj);
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

void rendering::DXUnlitRP::Create()
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

void rendering::DXUnlitRP::Prepare()
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

void rendering::DXUnlitRP::Execute()
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

void rendering::DXUnlitRP::Load(jobs::Job* done)
{
    core::utils::RunSync(done);
}

#undef THROW_ERROR

rendering::DXUnlitRP::DXUnlitRP() :
    RenderPass(DXUnlitRPMeta::GetInstance())
{
    CacheObjects();
    Create();
}

rendering::DXUnlitRP::~DXUnlitRP()
{
    if (m_commandListsCache)
    {
        delete[] m_commandListsCache;
    }
}


void rendering::DXUnlitRP::RenderUnlit()
{
    DXMaterial* errorMat = m_repo->GetMaterial("error");
    if (errorMat)
    {
        errorMat->ResetCommandLists();
    }

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
                DXMaterial* mat = m_repo->GetMaterial(*it);
                if (!mat)
                {
                    continue;
                }

                if (mat->GetMeta().HasTag(DXUnlitMaterialMetaTag::GetInstance()))
                {
                    mat->ResetCommandLists();
                }
            }
        }
    }

    std::list<ID3D12CommandList*> unlitLists;
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
                DXMaterial* mat = m_repo->GetMaterial(*matOverrideIt);
                ++matOverrideIt;

                const DXScene::GeometryResources& geometryResources = curSceneResources.m_geometryResources.find(obj.m_geometry)->second;
                DXBuffer* vertBuf = geometryResources.m_vertexBuffer;
                DXBuffer* indexBuf = geometryResources.m_indexBuffer;
                DXBuffer* instanceBuf = geometryResources.m_instanceBuffer;

                if (!mat)
                {
                    mat = errorMat;
                }

                if (!mat)
                {
                    continue;
                }

                if (!mat->GetMeta().HasTag(DXUnlitMaterialMetaTag::GetInstance()))
                {
                    continue;
                }
                
                unlitLists.push_back(mat->GenerateCommandList(
                    *vertBuf,
                    *indexBuf,
                    *instanceBuf,
                    (*it).indexOffset,
                    (*it).indexCount,
                    instanceIndex));
            }
        }
    }

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