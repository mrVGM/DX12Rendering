#include "DXCommandQueue.h"

#include "DXCommandQueueMeta.h"

#include "RenderUtils.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    errorMessage = error;\
    return false;\
}

bool rendering::DXCommandQueue::Create(std::string& errorMessage)
{
    using Microsoft::WRL::ComPtr;

    DXDevice* dxDevice = rendering::utils::GetDevice();
    if (!dxDevice)
    {
        errorMessage = "No device found!";
        return false;
    }


    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    THROW_ERROR(
        dxDevice->GetDevice().CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
        "Can't create Command QUEUE!")

    return true;
}

ID3D12CommandQueue* rendering::DXCommandQueue::GetCommandQueue()
{
    return m_commandQueue.Get();
}

#undef THROW_ERROR

rendering::DXCommandQueue::DXCommandQueue() :
    BaseObject(DXCommandQueueMeta::GetInstance())
{
    std::string error;
    bool res = Create(error);

    if (!res)
    {
        std::cerr << error << std::endl;
    }
}

rendering::DXCommandQueue::~DXCommandQueue()
{
}