#include "DXShadowMapMaterial.h"

#include "d3dx12.h"

#include "DXShadowMapMaterialMeta.h"

#include "CoreUtils.h"

#include "DXShader.h"

#include "DXBufferMeta.h"
#include "DXHeap.h"
#include "DXTexture.h"

#include "BaseObjectContainer.h"

#include "DXBuffer.h"

#include "DXDescriptorHeap.h"

#include "utils.h"

#include "ICamera.h"
#include "ICameraMeta.h"

namespace
{
    rendering::CascadedSM* m_cascadedSM = nullptr;

    rendering::DXDevice* m_device = nullptr;
    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXMutableBuffer* m_cameraBuffer = nullptr;

    void CacheObjects()
    {
        using namespace rendering;

        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }

        if (!m_swapChain)
        {
            m_swapChain = core::utils::GetSwapChain();
        }

        if (!m_cascadedSM)
        {
            m_cascadedSM = cascaded::GetCascadedSM();
        }

        if (!m_cameraBuffer)
        {
            m_cameraBuffer = cascaded::GetCameraBuffer();
        }
    }
}

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

void rendering::DXShadowMapMaterial::CreatePipelineStateAndRootSignatureForStaticMesh()
{
    DXDevice* device = m_device;

    using Microsoft::WRL::ComPtr;
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER1 rootParameters[3];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);
        rootParameters[2].InitAsConstantBufferView(2, 0);

        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

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
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs = nullptr;
        unsigned int inputElementsCount = 0;

        core::utils::Get3DMaterialInputLayout(inputElementDescs, inputElementsCount);

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementsCount };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

void rendering::DXShadowMapMaterial::CreatePipelineStateAndRootSignatureForSkeletalMesh()
{
    using Microsoft::WRL::ComPtr;
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER1 rootParameters[5];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);
        rootParameters[2].InitAsConstantBufferView(2, 0);
        rootParameters[3].InitAsShaderResourceView(0, 0);
        rootParameters[4].InitAsShaderResourceView(1, 0);

        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

            THROW_ERROR(
                m_device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatureForSkeletalMesh)),
                "Can't create a root signature!")
    }


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs = nullptr;
        unsigned int inputElementsCount = 0;

        core::utils::Get3DSkeletalMeshMaterialInputLayout(inputElementDescs, inputElementsCount);

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementsCount };
        psoDesc.pRootSignature = m_rootSignatureForSkeletalMesh.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShaderForSkeletalMesh.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            m_device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateForSkeletalMesh)),
            "Can't create Graphics Pipeline State!")
    }
}

rendering::DXShadowMapMaterial::DXShadowMapMaterial(const rendering::DXShader& vertexShader, const DXShader& vertexShaderForSkeletalMesh, const rendering::DXShader& pixelShader, int smSlot) :
    DXMaterial(DXShadowMapMaterialMeta::GetInstance(), vertexShader, pixelShader),
    m_vertexShaderForSkeletalMesh(vertexShaderForSkeletalMesh),
    m_smSlot(smSlot)
{
    CacheObjects();

    CreatePipelineStateAndRootSignatureForStaticMesh();
    CreatePipelineStateAndRootSignatureForSkeletalMesh();
}

rendering::DXShadowMapMaterial::~DXShadowMapMaterial()
{
}

ID3D12CommandList* rendering::DXShadowMapMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    DXTexture* shadowMapTexture = m_cascadedSM->GetShadowMap(0);

    m_commandLists.push_back(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>());
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = m_commandLists.back();

    THROW_ERROR(
        m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&commandList)),
        "Can't reset Command List!")

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, m_cameraBuffer->GetBuffer()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, m_cascadedSM->GetSettingsBuffer()->GetBuffer()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(2, m_materialSettingsBuffer->GetBuffer()->GetGPUVirtualAddress());
    
    {
        UINT texWidth = shadowMapTexture->GetTextureDescription().Width;
        UINT texHeight = shadowMapTexture->GetTextureDescription().Height;

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, texWidth, texHeight);
        CD3DX12_RECT scissorRect(0, 0, texWidth, texHeight);

        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = m_cascadedSM->GetDSDescriptorHeap()->GetDescriptorHandle(m_smSlot);
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        m_cascadedSM->GetSMDescriptorHeap()->GetDescriptorHandle(m_smSlot),
    };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, &dsHandle);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    realVertexBufferView.BufferLocation = vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer.GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer.GetBufferSize();

    D3D12_VERTEX_BUFFER_VIEW& instanceBufferView = vertexBufferViews[1];
    instanceBufferView.BufferLocation = instanceBuffer.GetBuffer()->GetGPUVirtualAddress();
    instanceBufferView.StrideInBytes = instanceBuffer.GetStride();
    instanceBufferView.SizeInBytes = instanceBuffer.GetBufferSize();

    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = indexBuffer.GetBuffer()->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = indexBuffer.GetBufferSize();

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);
    commandList->IASetIndexBuffer(&indexBufferView);

    int numInstances = instanceBuffer.GetBufferSize() / instanceBuffer.GetStride();
    commandList->DrawIndexedInstanced(
        indexCount,
        1,
        startIndex,
        0,
        instanceIndex
    );
    
    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")

    return commandList.Get();
}

ID3D12CommandList* rendering::DXShadowMapMaterial::GenerateCommandListForSkeletalMesh(
    const DXBuffer& vertexBuffer,
    const DXBuffer& jointsBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    const DXBuffer& skeletonBuffer,
    const DXBuffer& poseBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    DXTexture* shadowMapTexture = m_cascadedSM->GetShadowMap(0);

    m_commandLists.push_back(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>());
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = m_commandLists.back();

    THROW_ERROR(
        m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineStateForSkeletalMesh.Get(), IID_PPV_ARGS(&commandList)),
        "Can't reset Command List!")

    commandList->SetGraphicsRootSignature(m_rootSignatureForSkeletalMesh.Get());
    commandList->SetGraphicsRootConstantBufferView(0, m_cameraBuffer->GetBuffer()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, m_cascadedSM->GetSettingsBuffer()->GetBuffer()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(2, m_materialSettingsBuffer->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootShaderResourceView(3, skeletonBuffer.GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootShaderResourceView(4, poseBuffer.GetBuffer()->GetGPUVirtualAddress());

    {
        UINT texWidth = shadowMapTexture->GetTextureDescription().Width;
        UINT texHeight = shadowMapTexture->GetTextureDescription().Height;

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, texWidth, texHeight);
        CD3DX12_RECT scissorRect(0, 0, texWidth, texHeight);

        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = m_cascadedSM->GetDSDescriptorHeap()->GetDescriptorHandle(m_smSlot);
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        m_cascadedSM->GetSMDescriptorHeap()->GetDescriptorHandle(m_smSlot),
    };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, &dsHandle);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[3];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    realVertexBufferView.BufferLocation = vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer.GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer.GetBufferSize();

    D3D12_VERTEX_BUFFER_VIEW& instanceBufferView = vertexBufferViews[1];
    instanceBufferView.BufferLocation = instanceBuffer.GetBuffer()->GetGPUVirtualAddress();
    instanceBufferView.StrideInBytes = instanceBuffer.GetStride();
    instanceBufferView.SizeInBytes = instanceBuffer.GetBufferSize();

    D3D12_VERTEX_BUFFER_VIEW& jointsBufferView = vertexBufferViews[2];
    jointsBufferView.BufferLocation = jointsBuffer.GetBuffer()->GetGPUVirtualAddress();
    jointsBufferView.StrideInBytes = jointsBuffer.GetStride();
    jointsBufferView.SizeInBytes = jointsBuffer.GetBufferSize();

    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = indexBuffer.GetBuffer()->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = indexBuffer.GetBufferSize();

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);
    commandList->IASetIndexBuffer(&indexBufferView);

    int numInstances = instanceBuffer.GetBufferSize() / instanceBuffer.GetStride();
    commandList->DrawIndexedInstanced(
        indexCount,
        1,
        startIndex,
        0,
        instanceIndex
    );

    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")

        return commandList.Get();
}

void rendering::DXShadowMapMaterial::LoadBuffer(jobs::Job* done)
{
    struct Context
    {
        DXShadowMapMaterial* m_shadowMapMaterial = nullptr;

        DXBuffer* m_buffer = nullptr;
        DXHeap* m_heap = nullptr;

        jobs::Job* m_done = nullptr;
    };

    Context ctx;
    ctx.m_shadowMapMaterial = this;
    ctx.m_done = done;

    class PlaceBuffer : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        PlaceBuffer(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_buffer->Place(m_ctx.m_heap, 0);

            void* data = m_ctx.m_buffer->Map();
            int* intData = static_cast<int*>(data);
            *intData = m_ctx.m_shadowMapMaterial->m_smSlot;
            m_ctx.m_buffer->Unmap();

            m_ctx.m_shadowMapMaterial->m_materialSettingsBuffer = m_ctx.m_buffer;

            core::utils::RunSync(m_ctx.m_done);
        }
    };

    class CreateBuffer : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        CreateBuffer(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            UINT size = 256;
            UINT stride = size;

            m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
            m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_buffer->SetBufferStride(stride);

            m_ctx.m_heap = new DXHeap();
            m_ctx.m_heap->SetHeapSize(size);
            m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
            m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
            m_ctx.m_heap->Create();

            m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx));
        }
    };
    
    core::utils::RunSync(new CreateBuffer(ctx));
}

#undef THROW_ERROR