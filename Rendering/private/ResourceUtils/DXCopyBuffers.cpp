#include "ResourceUtils/DXCopyBuffers.h"

#include "ResourceUtils/DXCopyBuffersMeta.h"
#include "DXBuffer.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"
#include "Job.h"

#include "TemporaryBaseObject.h"

#include "RenderUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

void rendering::DXCopyBuffers::Create()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* device = rendering::utils::GetDevice();
    if (!device)
    {
        throw "No device found!";
    }

    THROW_ERROR(
        device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
        "Can't create Command Allocator!")

    THROW_ERROR(
        device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
        "Can't create Command List!")

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close command List!")
}

rendering::DXCopyBuffers::DXCopyBuffers() :
    BaseObject(DXCopyBuffersMeta::GetInstance())
{
    Create();
}

rendering::DXCopyBuffers::~DXCopyBuffers()
{
}

void rendering::DXCopyBuffers::Execute(
    DXBuffer& dst,
    D3D12_RESOURCE_STATES dstInitialState,
    const DXBuffer& src,
    D3D12_RESOURCE_STATES srcInitialState,
    jobs::Job* done
)
{
    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_commandList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(dst.GetBuffer(), dstInitialState, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(src.GetBuffer(), srcInitialState, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE)
        };
        m_commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    m_commandList->CopyResource(dst.GetBuffer(), src.GetBuffer());

    {
        CD3DX12_RESOURCE_BARRIER barrier[] =
        {
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(dst.GetBuffer(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, dstInitialState),
            CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(src.GetBuffer(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE, srcInitialState)
        };
        m_commandList->ResourceBarrier(_countof(barrier), barrier);
    }

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close Command List!")


    DXCopyCommandQueue* commandQueue = rendering::utils::GetCopyCommandQueue();
    ID3D12CommandList* copyCommandList[] = { m_commandList.Get() };
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

    DXFence* fence = new DXFence(DXFenceMeta::GetInstance());
    commandQueue->GetCommandQueue()->Signal(fence->GetFence(), 1);


    struct JobContext
    {
        TemporaryBaseObject* m_tempFence = nullptr;
        jobs::Job* m_done = nullptr;
    };

    TemporaryBaseObject* tempFence = new TemporaryBaseObject();

    class CopyJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        CopyJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            DXFence* fence = static_cast<DXFence*>(m_jobContext.m_tempFence->m_object);

            WaitFence waitFence(*fence);
            waitFence.Wait(1);

            delete m_jobContext.m_tempFence;

            jobs::JobSystem* loadSystem = utils::GetLoadJobSystem();
            loadSystem->ScheduleJob(m_jobContext.m_done);
        }
    };

    class CreateFenceJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        CreateFenceJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            m_jobContext.m_tempFence->m_object = new DXFence(DXFenceMeta::GetInstance());

            jobs::JobSystem* loadSystem = utils::GetLoadJobSystem();
            loadSystem->ScheduleJob(new CopyJob(m_jobContext));
        }
    };
    
    JobContext ctx { new TemporaryBaseObject(), done };

    jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
    mainSystem->ScheduleJob(new CreateFenceJob(ctx));
}

#undef THROW_ERROR
