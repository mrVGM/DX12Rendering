#include "DXCopyCommandQueue.h"

#include "DXCopyCommandQueueMeta.h"

#include "RenderUtils.h"


#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::DXCopyCommandQueue::DXCopyCommandQueue() :
	BaseObject(DXCopyCommandQueueMeta::GetInstance())
{
    Create();
}

rendering::DXCopyCommandQueue::~DXCopyCommandQueue()
{
}

void rendering::DXCopyCommandQueue::Create()
{
    DXDevice* dxDevice = rendering::utils::GetDevice();
    if (!dxDevice)
    {
        throw "No device found!";
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

    THROW_ERROR(
        dxDevice->GetDevice().CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
        "Can't create Command QUEUE!")
}

ID3D12CommandQueue* rendering::DXCopyCommandQueue::GetCommandQueue()
{
    return m_commandQueue.Get();
}

#undef THROW_ERROR