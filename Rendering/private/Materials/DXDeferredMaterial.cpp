#include "DXDeferredMaterial.h"

#include "d3dx12.h"

#include "DXDeferredMaterialMeta.h"

#include "RenderUtils.h"

#include "DXShader.h"

#include "DXBufferMeta.h"
#include "DXHeap.h"

#include "BaseObjectContainer.h"

#include "RenderPass/DXDeferredRP.h"
#include "RenderPass/DXDeferredRPMeta.h"

namespace
{
    rendering::DXDeferredRP* m_deferredRenderPass = nullptr;

    rendering::DXDeferredRP* GetDeferredRP()
    {
        using namespace rendering;
        if (m_deferredRenderPass)
        {
            return m_deferredRenderPass;
        }

        BaseObjectContainer& container = BaseObjectContainer::GetInstance();
        m_deferredRenderPass = static_cast<DXDeferredRP*>(container.GetObjectOfClass(DXDeferredRPMeta::GetInstance()));

        return m_deferredRenderPass;
    }
}

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::DXDeferredMaterial::DXDeferredMaterial(const rendering::DXShader& vertexShader, const rendering::DXShader& pixelShader) :
    DXMaterial(DXDeferredMaterialMeta::GetInstance(), vertexShader, pixelShader)
{
    DXDevice* device = utils::GetDevice();

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
        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);

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
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

            { "OBJECT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
            { "OBJECT_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
            { "OBJECT_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 3;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        psoDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        psoDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }

}

rendering::DXDeferredMaterial::~DXDeferredMaterial()
{
}

ID3D12CommandList* rendering::DXDeferredMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    DXDevice* device = utils::GetDevice();
    DXSwapChain* swapChain = utils::GetSwapChain();
    DXBuffer* camBuff = utils::GetCameraBuffer();

    ID3D12Resource* curRT = swapChain->GetCurrentRenderTarget();

    m_commandLists.push_back(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>());
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList = m_commandLists.back();

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&commandList)),
        "Can't reset Command List!")

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, camBuff->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, m_settingsBuffer->GetBuffer()->GetGPUVirtualAddress());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    DXDeferredRP* deferredRP = GetDeferredRP();
    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = utils::GetDSVDescriptorHeap()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] =
    {
        deferredRP->GetDescriptorHandleFor(DXDeferredRP::Diffuse),
        deferredRP->GetDescriptorHandleFor(DXDeferredRP::Normal),
        deferredRP->GetDescriptorHandleFor(DXDeferredRP::Position)
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

    commandList->IASetVertexBuffers(0, 2, vertexBufferViews);
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

void rendering::DXDeferredMaterial::CreateSettingsBuffer(jobs::Job* done)
{
    struct Context
    {
        DXDeferredMaterial* m_deferredMaterial = nullptr;

        DXBuffer* m_buffer = nullptr;
        DXHeap* m_heap = nullptr;

        jobs::Job* m_done = nullptr;
    };

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
            m_ctx.m_deferredMaterial->m_settingsBuffer = m_ctx.m_buffer;

            utils::RunSync(m_ctx.m_done);
        }
    };

    class CreateBufferAndHeap : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        CreateBufferAndHeap(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do()
        {
            m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
            m_ctx.m_heap = new DXHeap();

            m_ctx.m_buffer->SetBufferSizeAndFlags(256, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
            m_ctx.m_buffer->SetBufferStride(256);

            m_ctx.m_heap->SetHeapSize(256);
            m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD);
            m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);

            m_ctx.m_heap->Create();
            m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx));
        }
    };

    Context ctx{ this, nullptr, nullptr, done };
    utils::RunSync(new CreateBufferAndHeap(ctx));
}

rendering::DXBuffer* rendering::DXDeferredMaterial::GetSettingsBuffer()
{
    return m_settingsBuffer;
}

#undef THROW_ERROR