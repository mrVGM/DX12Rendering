#include "DXPostLightsCalculationsMaterial.h"

#include "DXPostLightsCalculationsMaterialMeta.h"


#include "DXShader.h"
#include "DXDescriptorHeap.h"

#include "BaseObjectContainer.h"

#include "DXBuffer.h"
#include "DXTexture.h"

#include "resources/DXLightsBufferMeta.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "DeferredRendering.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXDevice* m_device = nullptr;
    rendering::DXSwapChain* m_swapChain = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        BaseObjectContainer& container = BaseObjectContainer::GetInstance();

        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }

        if (!m_swapChain)
        {
            m_swapChain = core::utils::GetSwapChain();
        }
    }
}


rendering::DXPostLightsCalculationsMaterial::DXPostLightsCalculationsMaterial(const rendering::DXShader& vertexShader, const rendering::DXShader& pixelShader) :
    DXMaterial(DXPostLightsCalculationsMaterialMeta::GetInstance(), vertexShader, pixelShader)
{
    CacheObjects();
    CreatePipelineStateAndRootSignature();
    CreateDescriptorHeaps();
}

rendering::DXPostLightsCalculationsMaterial::~DXPostLightsCalculationsMaterial()
{
}

ID3D12CommandList* rendering::DXPostLightsCalculationsMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    m_commandLists.push_back(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>());
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = m_commandLists.back();

    DXDevice* device = m_device;
    DXSwapChain* swapChain = m_swapChain;

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&commandList)),
        "Can't reset Command List!")



    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap->GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    commandList->SetGraphicsRootDescriptorTable(0, descriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        swapChain->GetCurrentRTVDescriptor()
    };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[1];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    realVertexBufferView.BufferLocation = vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer.GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer.GetBufferSize();

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);

    commandList->DrawInstanced(6, 1, 0, 0);

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferAmbientLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferDiffuseLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferSpecularLitTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT)
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")

    return commandList.Get();
}

void rendering::DXPostLightsCalculationsMaterial::CreatePipelineStateAndRootSignature()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = m_device;

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

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
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::DXPostLightsCalculationsMaterial::CreateDescriptorHeaps()
{
    {
        std::list<DXTexture*> textures;
        textures.push_back(deferred::GetGBufferAmbientLitTex());
        textures.push_back(deferred::GetGBufferDiffuseLitTex());
        textures.push_back(deferred::GetGBufferSpecularLitTex());

        m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
    }
}

#undef THROW_ERROR