#include "DXPostProcessMaterial.h"
#include "DXPostProcessMaterialMeta.h"

#include "DXShader.h"
#include "DXDescriptorHeap.h"

#include "BaseObjectContainer.h"

#include "DXBuffer.h"
#include "DXTexture.h"

#include "resources/DXLightsBufferMeta.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "DeferredRendering.h"

#include "utils.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::Window* m_wnd = nullptr;
    rendering::DXDevice* m_device = nullptr;
    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXMutableBuffer* m_camBuffer = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        BaseObjectContainer& container = BaseObjectContainer::GetInstance();

        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }

        if (!m_wnd)
        {
            m_wnd = core::utils::GetWindow();
        }

        if (!m_swapChain)
        {
            m_swapChain = core::utils::GetSwapChain();
        }

        if (!m_camBuffer)
        {
            m_camBuffer = deferred::GetCameraBuffer();
        }
    }
}


rendering::DXPostProcessMaterial::DXPostProcessMaterial(const rendering::DXShader& vertexShader, const rendering::DXShader& pixelShader) :
    DXMaterial(DXPostProcessMaterialMeta::GetInstance(), vertexShader, pixelShader)
{
    CacheObjects();
    CreatePipelineStateAndRootSignature();
    CreateDescriptorHeaps();
}

rendering::DXPostProcessMaterial::~DXPostProcessMaterial()
{
}

ID3D12CommandList* rendering::DXPostProcessMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    if (!m_commandLists.empty())
    {
        return m_commandLists.back().Get();
    }

    DXDevice* device = m_device;

    m_commandLists.push_back(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>());
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = m_commandLists.back();

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&commandList)),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),

            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap->GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    commandList->SetGraphicsRootConstantBufferView(0, m_camBuffer->GetBuffer()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(1, descriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());

    UINT64 width = m_wnd->m_width;
    UINT64 height = m_wnd->m_height;

    CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    CD3DX12_RECT scissorRect = CD3DX12_RECT(0, 0, width, height);


    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        m_swapChain->GetCurrentRTVDescriptor(),
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
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferNormalTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET,  D3D12_RESOURCE_STATE_PRESENT),
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")


    return commandList.Get();
}

void rendering::DXPostProcessMaterial::CreatePipelineStateAndRootSignature()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = core::utils::GetDevice();

    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        D3D12_STATIC_SAMPLER_DESC unfilteredSampler = {};
        unfilteredSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        unfilteredSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        unfilteredSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        unfilteredSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        unfilteredSampler.MipLODBias = 0;
        unfilteredSampler.MaxAnisotropy = 0;
        unfilteredSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        unfilteredSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        unfilteredSampler.MinLOD = 0.0f;
        unfilteredSampler.MaxLOD = D3D12_FLOAT32_MAX;
        unfilteredSampler.ShaderRegister = 0;
        unfilteredSampler.RegisterSpace = 0;
        unfilteredSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_STATIC_SAMPLER_DESC samplers[] = { unfilteredSampler };

        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL);
        
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplers), samplers, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

        THROW_ERROR(
            device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)),
            "Can't create a root signature!")
    }


    {
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        
        {
            psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
        }
        
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

void rendering::DXPostProcessMaterial::CreateDescriptorHeaps()
{
    {
        std::list<DXTexture*> textures;
        textures.push_back(deferred::GetGBufferNormalTex());

        m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
    }
}

#undef THROW_ERROR