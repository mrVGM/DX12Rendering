#include "RenderPass/DXUnlitRP.h"

#include "RenderPass/DXUnlitRPMeta.h"
#include "RenderUtils.h"

#include <set>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

void rendering::DXUnlitRP::Create()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = rendering::utils::GetDevice();
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
    DXSwapChain* swapChain = rendering::utils::GetSwapChain();
    if (!swapChain)
    {
        throw "No Swap Chain found!";
    }

    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    THROW_ERROR(
        m_startList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_startList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_startList->Close(),
        "Can't close command List!")

    THROW_ERROR(
        m_endList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_endList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_endList->Close(),
        "Can't close Command List!")



}

void rendering::DXUnlitRP::Execute()
{
    DXCommandQueue* commandQueue = rendering::utils::GetCommandQueue();

    {
        ID3D12CommandList* ppCommandLists[] = { m_startList.Get() };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    RenderUnlit();

    {
        ID3D12CommandList* ppCommandLists[] = { m_endList.Get() };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

#undef THROW_ERROR

rendering::DXUnlitRP::DXUnlitRP() :
    RenderPass(DXUnlitRPMeta::GetInstance())
{
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
    DXScene* scene = utils::GetScene();
    DXMaterial* mat = utils::GetUnlitMaterial();

    mat->ResetCommandLists();

    DXMaterialRepo* repo = utils::GetMaterialRepo();
    DXMaterial* errorMat = repo->GetMaterial("error");

    std::list<ID3D12CommandList*> unlitLists;
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
            
            while (obj.m_materialOverrides.size() < geo.m_materials.size())
            {
                obj.m_materialOverrides.push_back("");
            }

            auto matOverrideIt = obj.m_materialOverrides.begin();

            for (auto it = geo.m_materials.begin(); it != geo.m_materials.end(); ++it)
            {
                DXMaterial* mat = repo->GetMaterial(*matOverrideIt);
                ++matOverrideIt;

                DXBuffer* vertBuf = curSceneResources.m_vertexBuffers.find(obj.m_geometry)->second;
                DXBuffer* indexBuf = curSceneResources.m_indexBuffers.find(obj.m_geometry)->second;
                DXBuffer* instanceBuf = curSceneResources.m_instanceBuffers.find(obj.m_geometry)->second;

                if (mat)
                {
                    continue;
                }
                
                unlitLists.push_back(errorMat->GenerateCommandList(
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

    DXCommandQueue* commandQueue = utils::GetCommandQueue();
    commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
}