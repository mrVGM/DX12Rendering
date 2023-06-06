#include "DXShadowMaskFilterMaterial.h"
#include "DXShadowMaskFilterMaterialMeta.h"

#include "DXShader.h"
#include "DXDescriptorHeap.h"

#include "BaseObjectContainer.h"

#include "DXBuffer.h"
#include "DXTexture.h"

#include "DXCameraBufferMeta.h"
#include "resources/DXLightsBufferMeta.h"
#include "resources/DXShadowMapMeta.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "DeferredRendering.h"

#include "resources/DXSMSettingsBufferMeta.h"

#include "utils.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXBuffer* m_cameraBuffer = nullptr;
    rendering::DXBuffer* m_lightsBuffer = nullptr;
    rendering::DXBuffer* m_smSettingsBuffer = nullptr;
    rendering::DXTexture* m_shadowMap = nullptr;

    rendering::CascadedSM* m_cascadedSM = nullptr;
    rendering::Window* m_wnd = nullptr;

    rendering::DXDevice* m_device = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        BaseObjectContainer& container = BaseObjectContainer::GetInstance();

        if (!m_cameraBuffer)
        {
            BaseObject* obj = container.GetObjectOfClass(DXCameraBufferMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Camera Buffer!";
            }

            m_cameraBuffer = static_cast<DXBuffer*>(obj);
        }

        if (!m_lightsBuffer)
        {
            BaseObject* obj = container.GetObjectOfClass(DXLightsBufferMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Lights Buffer!";
            }

            m_lightsBuffer = static_cast<DXBuffer*>(obj);
        }

        if (!m_shadowMap)
        {
            BaseObject* obj = container.GetObjectOfClass(DXShadowMapMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Shadow Map!";
            }

            m_shadowMap = static_cast<DXTexture*>(obj);
        }

        if (!m_smSettingsBuffer)
        {
            BaseObject* obj = container.GetObjectOfClass(DXSMSettingsBufferMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Shadow Map Settings!";
            }

            m_smSettingsBuffer = static_cast<DXBuffer*>(obj);
        }


        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }


        if (!m_cascadedSM)
        {
            m_cascadedSM = deferred::GetCascadedSM();
        }

        if (!m_wnd)
        {
            m_wnd = core::utils::GetWindow();
        }
    }
}


rendering::DXShadowMaskFilterMaterial::DXShadowMaskFilterMaterial(const rendering::DXShader& vertexShader, const rendering::DXShader& pixelShader, int renderTargetTex) :
    DXMaterial(DXShadowMaskFilterMaterialMeta::GetInstance(), vertexShader, pixelShader),
    m_rtvTexIndex(renderTargetTex),
    m_srvTexIndex((renderTargetTex + 1) % 2)
{
    CacheObjects();
    CreatePipelineStateAndRootSignature();
    CreateDescriptorHeaps();
}

rendering::DXShadowMaskFilterMaterial::~DXShadowMaskFilterMaterial()
{
}

ID3D12CommandList* rendering::DXShadowMaskFilterMaterial::GenerateCommandList(
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
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMap(0)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),

            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMask(m_rtvTexIndex)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMask(m_srvTexIndex)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    {
        const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        commandList->ClearRenderTargetView(m_rtvHeap->GetDescriptorHandle(0), clearColor, 0, nullptr);
    }

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvHeap->GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    commandList->SetGraphicsRootConstantBufferView(0, m_cameraBuffer->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, m_smSettingsBuffer->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(2, descriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());


    UINT64 width = m_wnd->m_width;
    UINT64 height = m_wnd->m_height;

    CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    CD3DX12_RECT scissorRect = CD3DX12_RECT(0, 0, width, height);


    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        m_rtvHeap->GetDescriptorHandle(0),
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
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(deferred::GetGBufferPositionTex()->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMap(0)->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),

            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMask(m_rtvTexIndex)->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_cascadedSM->GetShadowMask(m_srvTexIndex)->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
        };
        commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")


    return commandList.Get();
}

void rendering::DXShadowMaskFilterMaterial::CreatePipelineStateAndRootSignature()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = core::utils::GetDevice();

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
        CD3DX12_ROOT_PARAMETER1 rootParameters[3];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);
        rootParameters[2].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL);

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
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = false;
        psoDesc.DepthStencilState.StencilEnable = false;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::DXShadowMaskFilterMaterial::CreateDescriptorHeaps()
{
    {
        std::list<DXTexture*> textures;
        textures.push_back(deferred::GetGBufferPositionTex());
        textures.push_back(m_cascadedSM->GetShadowMap(0));
        textures.push_back(m_cascadedSM->GetShadowMask(m_srvTexIndex));

        m_srvHeap = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
    }

    {
        std::list<DXTexture*> textures;
        textures.push_back(m_cascadedSM->GetShadowMask(m_rtvTexIndex));

        m_rtvHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
    }
}

#undef THROW_ERROR