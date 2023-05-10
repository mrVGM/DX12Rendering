#include "RenderPass/DXClearDSTRP.h"

#include "RenderPass/DXClearDSTRPMeta.h"
#include "RenderUtils.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
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

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = utils::GetDSVDescriptorHeap()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
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
    utils::RunSync(done);
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