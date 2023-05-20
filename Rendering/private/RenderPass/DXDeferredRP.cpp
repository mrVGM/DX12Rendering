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

#include <set>
#include <list>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}



namespace
{
    rendering::LightsManager* GetLightsManager()
    {
        using namespace rendering;

        LightsManager* lightsManager = utils::GetLightsManager();
        if (!lightsManager)
        {
            new LightsManager();
        }
        lightsManager = utils::GetLightsManager();

        return lightsManager;
    }
}



rendering::DXDeferredRP::DXDeferredRP() :
    RenderPass(DXDeferredRPMeta::GetInstance()),
    m_vertexShader(*rendering::shader_repo::GetDeferredRPVertexShader()),
    m_pixelShader(*rendering::shader_repo::GetDeferredRPPixelShader()),
    m_postLightingVertexShader(*rendering::shader_repo::GetDeferredRPVertexShader()),
    m_postLightingPixelShader(*rendering::shader_repo::GetDeferredRPPostLightingPixelShader())
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = utils::GetDevice();

    {
        THROW_ERROR(
            device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_lightCalculationsAllocator)),
            "Can't create Command Allocator!")

        THROW_ERROR(
            device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_lightCalculationsAllocator.Get(), nullptr, IID_PPV_ARGS(&m_startList)),
            "Can't reset Command List!")

        THROW_ERROR(
            m_startList->Close(),
            "Can't close Command List!")

        THROW_ERROR(
            device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_lightCalculationsAllocator.Get(), nullptr, IID_PPV_ARGS(&m_endList)),
            "Can't reset Command List!")

        THROW_ERROR(
            m_endList->Close(),
            "Can't close Command List!")
    }

    {
        THROW_ERROR(
            device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_postlightCalculationsAllocator)),
            "Can't create Command Allocator!")

        THROW_ERROR(
            device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_postlightCalculationsAllocator.Get(), nullptr, IID_PPV_ARGS(&m_postLightingList)),
            "Can't reset Command List!")

        THROW_ERROR(
            m_postLightingList->Close(),
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

D3D12_CPU_DESCRIPTOR_HANDLE rendering::DXDeferredRP::GetDescriptorHandleFor(GBufferLitTexType texType)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvLitHeap->GetDescriptorHandle(texType);
    return handle;
}


void rendering::DXDeferredRP::CreateLightCalculationsPipelineStageAndRootSignature()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = utils::GetDevice();

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0, 0);
        CD3DX12_ROOT_PARAMETER1 rootParameters[4];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);
        rootParameters[2].InitAsConstantBufferView(2, 0);
        rootParameters[3].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

        THROW_ERROR(
            device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)),
            "Can't create a root signature!")
    }


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        for (int i = 0; i < 3; ++i)
        {
            psoDesc.BlendState.RenderTarget[i].BlendEnable = TRUE;
            psoDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = 0x0f;
        }

        psoDesc.DepthStencilState.DepthEnable = false;
        psoDesc.DepthStencilState.StencilEnable = false;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 3;

        for (int i = 0; i < 3; ++i)
        {
            psoDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::DXDeferredRP::CreatePostLightingPipelineStageAndRootSignature()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = utils::GetDevice();

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0);
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

        THROW_ERROR(
            device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_postLigtingRootSignature)),
            "Can't create a root signature!")
    }


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_postLigtingRootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_postLightingVertexShader.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_postLightingPixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
        psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
        psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
        psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
        psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;

        psoDesc.DepthStencilState.DepthEnable = false;
        psoDesc.DepthStencilState.StencilEnable = false;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_postLigtingPipelineState)),
            "Can't create Graphics Pipeline State!")
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
    LightsManager* lightsManager = GetLightsManager();

    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferDiffuseTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularTex());
    textures.push_back(rendering::deferred::GetGBufferNormalTex());
    textures.push_back(rendering::deferred::GetGBufferPositionTex());
    textures.push_back(lightsManager->GetShadowMap());

    m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::CreateRTVLitHeap()
{
    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferAmbientLitTex());
    textures.push_back(rendering::deferred::GetGBufferDiffuseLitTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularLitTex());

    m_rtvLitHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::CreateSRVLitHeap()
{
    std::list<DXTexture*> textures;
    textures.push_back(rendering::deferred::GetGBufferAmbientLitTex());
    textures.push_back(rendering::deferred::GetGBufferDiffuseLitTex());
    textures.push_back(rendering::deferred::GetGBufferSpecularLitTex());
    m_srvLitHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::DXDeferredRP::PrepareEndList()
{
    if (m_endListPrepared)
    {
        return;
    }

    DXDevice* device = utils::GetDevice();
    DXSwapChain* swapChain = utils::GetSwapChain();

    THROW_ERROR(
        m_endList->Reset(m_lightCalculationsAllocator.Get(), m_pipelineState.Get()),
        "Can't reset Command List!")

    LightsManager* lightsManager = GetLightsManager();

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(lightsManager->GetShadowMap()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
        };
        m_endList->ResourceBarrier(_countof(barrier), barrier);
    }

    {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_endList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferLitTexType::AmbientLit), clearColor, 0, nullptr);
        m_endList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferLitTexType::DiffuseLit), clearColor, 0, nullptr);
        m_endList->ClearRenderTargetView(GetDescriptorHandleFor(GBufferLitTexType::SpecularLit), clearColor, 0, nullptr);
    }

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
        };
        m_endList->ResourceBarrier(_countof(barrier), barrier);
    }


    m_endList->SetGraphicsRootSignature(m_rootSignature.Get());
    ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap->GetDescriptorHeap() };
    m_endList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    m_endList->SetGraphicsRootConstantBufferView(0, utils::GetCameraBuffer()->GetBuffer()->GetGPUVirtualAddress());
    m_endList->SetGraphicsRootConstantBufferView(1, m_lightsBuffer->GetBuffer()->GetGPUVirtualAddress());
    m_endList->SetGraphicsRootConstantBufferView(2, lightsManager->GetSMSettingsBuffer()->GetBuffer()->GetGPUVirtualAddress());
    m_endList->SetGraphicsRootDescriptorTable(3, descriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());

    m_endList->RSSetViewports(1, &swapChain->GetViewport());
    m_endList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        GetDescriptorHandleFor(DXDeferredRP::GBufferLitTexType::AmbientLit),
        GetDescriptorHandleFor(DXDeferredRP::GBufferLitTexType::DiffuseLit),
        GetDescriptorHandleFor(DXDeferredRP::GBufferLitTexType::SpecularLit),
    };
    m_endList->OMSetRenderTargets(_countof(handles), handles, FALSE, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[1];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    DXBuffer* vertexBuffer = rendering::deferred::GetRenderTextureVertexBuffer();
    realVertexBufferView.BufferLocation = vertexBuffer->GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer->GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer->GetBufferSize();

    m_endList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_endList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);

    m_endList->DrawInstanced(6, 1, 0, 0);

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),

            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
        };
        m_endList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        m_endList->Close(),
        "Can't close Command List!")

    m_endListPrepared = true;
}

void rendering::DXDeferredRP::PrepareStartList()
{
    if (m_startListPrepared)
    {
        return;
    }

    DXDevice* device = utils::GetDevice();

    THROW_ERROR(
        m_startList->Reset(m_lightCalculationsAllocator.Get(), nullptr),
        "Can't reset Command List!")

    LightsManager* lightsManager = GetLightsManager();
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

void rendering::DXDeferredRP::PreparePostLightingList()
{
    DXDevice* device = utils::GetDevice();
    DXSwapChain* swapChain = utils::GetSwapChain();

    THROW_ERROR(
        m_postlightCalculationsAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_postLightingList->Reset(m_postlightCalculationsAllocator.Get(), m_postLigtingPipelineState.Get()),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        };
        m_postLightingList->ResourceBarrier(_countof(barrier), barrier);
    }

    m_postLightingList->SetGraphicsRootSignature(m_postLigtingRootSignature.Get());
    ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvLitHeap->GetDescriptorHeap() };
    m_postLightingList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    m_postLightingList->SetGraphicsRootDescriptorTable(0, descriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());

    m_postLightingList->RSSetViewports(1, &swapChain->GetViewport());
    m_postLightingList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        swapChain->GetCurrentRTVDescriptor()
    };
    m_postLightingList->OMSetRenderTargets(_countof(handles), handles, FALSE, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[1];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    DXBuffer* vertexBuffer = rendering::deferred::GetRenderTextureVertexBuffer();
    realVertexBufferView.BufferLocation = vertexBuffer->GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer->GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer->GetBufferSize();

    m_postLightingList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_postLightingList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);

    m_postLightingList->DrawInstanced(6, 1, 0, 0);

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT)
        };
        m_postLightingList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        m_postLightingList->Close(),
        "Can't close Command List!")
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
    PrepareEndList();
    PreparePostLightingList();
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
        ID3D12CommandList* ppCommandLists[] = { m_endList.Get() };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    {
        ID3D12CommandList* ppCommandLists[] = { m_postLightingList.Get() };
        commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
}

void rendering::DXDeferredRP::LoadLightsBuffer(jobs::Job* done)
{
    LightsManager* lightsManager = GetLightsManager();
    struct Context
    {
        DXDeferredRP* m_deferredRP = nullptr;
        LightsManager* m_lightsManager = nullptr;
        jobs::Job* m_done = nullptr;
    };

    class LoadBuffer : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        LoadBuffer(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_deferredRP->m_lightsBuffer = m_ctx.m_lightsManager->GetLightsBuffer();
            utils::RunSync(m_ctx.m_done);
        }
    };

    Context ctx
    {
        this,
        lightsManager,
        done
    };

    lightsManager->LoadLightsBuffer(new LoadBuffer(ctx));
}

void rendering::DXDeferredRP::LoadShadowMap(jobs::Job* done)
{
    LightsManager* lightsManager = GetLightsManager();

    struct Context
    {
        DXDeferredRP* m_deferredRP = nullptr;
        LightsManager* m_lightsManager = nullptr;
        jobs::Job* m_done = nullptr;
    };

    class LoadShadowMapTex : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        LoadShadowMapTex(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_deferredRP->m_shadowMapTex = m_ctx.m_lightsManager->GetShadowMap();
            utils::RunSync(m_ctx.m_done);
        }
    };

    Context ctx
    {
        this,
        lightsManager,
        done
    };

    lightsManager->LoadShadowMap(new LoadShadowMapTex(ctx));
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

            m_ctx.m_deferredRP->CreateLightCalculationsPipelineStageAndRootSignature();
            m_ctx.m_deferredRP->CreatePostLightingPipelineStageAndRootSignature();
            
            m_ctx.m_deferredRP->CreateRTVHeap();
            m_ctx.m_deferredRP->CreateSRVHeap();

            m_ctx.m_deferredRP->CreateRTVLitHeap();
            m_ctx.m_deferredRP->CreateSRVLitHeap();

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