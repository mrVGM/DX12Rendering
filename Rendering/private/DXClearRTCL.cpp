#include "DXClearRTCL.h"

#include "DXClearRTCLMeta.h"
#include "RenderUtils.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    errorMessage = error;\
    return false;\
}

bool rendering::DXClearRTCL::Create(std::string& errorMessage)
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = rendering::utils::GetDevice();
    if (!device)
    {
        errorMessage = "No device found!";
        return false;
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

    return true;
}

bool rendering::DXClearRTCL::Populate(std::string& errorMessage)
{
    DXSwapChain* swapChain = rendering::utils::GetSwapChain();
    if (!swapChain)
    {
        errorMessage = "No Swap Chain found!";
        return false;
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
        m_commandList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier);
    }

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(swapChain->GetCurrentRTVDescriptor(), clearColor, 0, nullptr);

    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
    }

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close Command List!")

    return true;
}

bool rendering::DXClearRTCL::Execute(std::string& errorMessage)
{
    DXCommandQueue* commandQueue = rendering::utils::GetCommandQueue();
    if (!commandQueue)
    {
        errorMessage = "No Command Queue found!";
        return false;
    }

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    return true;
}

#undef THROW_ERROR

rendering::DXClearRTCL::DXClearRTCL() :
    BaseObject(DXClearRTCLMeta::GetInstance())
{
    std::string error;
    bool res = Create(error);

    if (!res)
    {
        std::cerr << error << std::endl;
    }
}

rendering::DXClearRTCL::~DXClearRTCL()
{
}