#include "DXCommandQueue.h"

#include "DXCommandQueueMeta.h"

#include "CoreUtils.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

void rendering::DXCommandQueue::Create()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* dxDevice = rendering::utils::GetDevice();
    if (!dxDevice)
    {
        throw "No device found!";
    }


    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    THROW_ERROR(
        dxDevice->GetDevice().CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
        "Can't create Command QUEUE!")
}

ID3D12CommandQueue* rendering::DXCommandQueue::GetCommandQueue()
{
    return m_commandQueue.Get();
}

#undef THROW_ERROR

rendering::DXCommandQueue::DXCommandQueue() :
    BaseObject(DXCommandQueueMeta::GetInstance())
{
    Create();
}

rendering::DXCommandQueue::~DXCommandQueue()
{
}