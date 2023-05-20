#include "RenderPass/DXClearDSTRP.h"

#include "RenderPass/DXClearDSTRPMeta.h"
#include "RenderUtils.h"

#include "DXTexture.h"
#include "DXDepthStencilTextureMeta.h"
#include "DXDepthStencilDescriptorHeapMeta.h"

#include "DXHeap.h"

#include "BaseObjectContainer.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXDescriptorHeap* m_depthStencilDescriptorHeap = nullptr;
}

void rendering::DXClearDSTRP::Create()
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
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
        "Can't create Command List!")

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close command List!")
}

void rendering::DXClearDSTRP::Prepare()
{
    if (m_prepared)
    {
        return;
    }

    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_commandList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_depthStencilDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close Command List!")
}

void rendering::DXClearDSTRP::Execute()
{
    DXCommandQueue* commandQueue = rendering::utils::GetCommandQueue();
    if (!commandQueue)
    {
        throw "No Command Queue found!";
    }

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void rendering::DXClearDSTRP::Load(jobs::Job* done)
{
    struct Context
    {
        DXTexture* m_texture = nullptr;
        DXHeap* m_heap = nullptr;
        jobs::Job* m_done;
    };

    Context ctx;
    ctx.m_done = done;

    class CreateDescriptorHeap : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        CreateDescriptorHeap(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_depthStencilDescriptorHeap = DXDescriptorHeap::CreateDSVDescriptorHeap(DXDepthStencilDescriptorHeapMeta::GetInstance(), *m_ctx.m_texture);
            utils::RunSync(m_ctx.m_done);
        }
    };

    class HeapResident : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        HeapResident(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_texture->Place(*m_ctx.m_heap, 0);
            utils::RunSync(new CreateDescriptorHeap(m_ctx));
        }
    };

    class CreateObjects : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        CreateObjects(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            Window* window = utils::GetWindow();

            m_ctx.m_texture = DXTexture::CreateDepthStencilTexture(DXDepthStencilTextureMeta::GetInstance(), window->m_width, window->m_height);
            m_ctx.m_heap = new DXHeap();

            m_ctx.m_heap->SetHeapSize(m_ctx.m_texture->GetTextureAllocationInfo().SizeInBytes);
            m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
            m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
            m_ctx.m_heap->Create();

            m_ctx.m_heap->MakeResident(new HeapResident(m_ctx));
        }
    };

    utils::RunSync(new CreateObjects(ctx));
}


rendering::DXClearDSTRP::DXClearDSTRP() :
    RenderPass(DXClearDSTRPMeta::GetInstance())
{
    Create();
}

rendering::DXClearDSTRP::~DXClearDSTRP()
{
}


#undef THROW_ERROR