#include "DXClearRTRP.h"

#include "DXClearRTRPMeta.h"

#include "CoreUtils.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXDevice* m_device = nullptr;
    rendering::DXSwapChain* m_swapChain = nullptr;
    rendering::DXCommandQueue* m_commandQueue = nullptr;

    void CacheObjects()
    {
        if (!m_device)
        {
            m_device = rendering::core::utils::GetDevice();
        }

        if (!m_swapChain)
        {
            m_swapChain = rendering::core::utils::GetSwapChain();
        }

        if (!m_commandQueue)
        {
            m_commandQueue = rendering::core::utils::GetCommandQueue();
        }
    }
}

void rendering::DXClearRTRP::Create()
{
    using Microsoft::WRL::ComPtr;

    THROW_ERROR(
        m_device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
        "Can't create Command Allocator!")

    THROW_ERROR(
        m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
        "Can't create Command List!")

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close command List!")
}

void rendering::DXClearRTRP::Prepare()
{
    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_commandList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier);
    }

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(m_swapChain->GetCurrentRTVDescriptor(), clearColor, 0, nullptr);

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close Command List!")
}

void rendering::DXClearRTRP::Execute()
{
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void rendering::DXClearRTRP::Load(jobs::Job* done)
{
    core::utils::RunSync(done);
}

#undef THROW_ERROR

rendering::DXClearRTRP::DXClearRTRP() :
    RenderPass(DXClearRTRPMeta::GetInstance())
{
    CacheObjects();
    Create();
}

rendering::DXClearRTRP::~DXClearRTRP()
{
}