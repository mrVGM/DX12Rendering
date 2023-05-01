#include "DXCommandQueue.h"

#include "BaseObjectContainer.h"

#include "DXCommandQueueMeta.h"
#include "DXDeviceMeta.h"
#include "DXDevice.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    errorMessage = error;\
    return false;\
}

bool rendering::DXCommandQueue::Create(std::string& errorMessage)
{
    using Microsoft::WRL::ComPtr;

    BaseObject* obj = BaseObjectContainer::GetInstance().GetObjectOfClass(DXDeviceMeta::GetInstance());

    if (!obj)
    {
        errorMessage = "No device found!";
        return false;
    }

    DXDevice* dxDevice = static_cast<DXDevice*>(obj);

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