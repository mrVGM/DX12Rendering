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

void rendering::DXCopyBuffers::ExecuteCommandList(
    DXBuffer& dst,
    const DXBuffer& src,
    ID3D12Fence* fence,
    UINT64 signal)
{
    THROW_ERROR(
        m_commandAllocator->Reset(),
        "Can't reset Command Allocator!")

    THROW_ERROR(
        m_commandList->Reset(m_commandAllocator.Get(), nullptr),
        "Can't reset Command List!")

    m_commandList->CopyResource(dst.GetBuffer(), src.GetBuffer());

    THROW_ERROR(
        m_commandList->Close(),
        "Can't close Command List!")


    DXCopyCommandQueue* commandQueue = rendering::utils::GetCopyCommandQueue();
    ID3D12CommandList* copyCommandList[] = { m_commandList.Get() };
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

    if (fence)
    {
        commandQueue->GetCommandQueue()->Signal(fence, signal);
    }
}

void rendering::DXCopyBuffers::Execute(
    DXBuffer& dst,
    const DXBuffer& src,
    jobs::Job* done,
    jobs::JobSystem* jobSystem)
{
    struct JobContext
    {
        DXCopyBuffers* m_dxCopyBuffers = nullptr;
        DXBuffer* m_dst = nullptr;
        const DXBuffer* m_src = nullptr;

        TemporaryBaseObject* m_tempFence = nullptr;
        jobs::Job* m_done = nullptr;
        jobs::JobSystem* m_jobSystem = nullptr;
    };

    TemporaryBaseObject* tempFence = new TemporaryBaseObject();
    JobContext ctx{ this, &dst, &src, tempFence, done, jobSystem };

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
            UINT64 signal = 1;
            m_jobContext.m_dxCopyBuffers->ExecuteCommandList(*m_jobContext.m_dst, *m_jobContext.m_src, fence->GetFence(), 1);
            
            WaitFence waitFence(*fence);
            waitFence.Wait(signal);

            delete m_jobContext.m_tempFence;

            m_jobContext.m_jobSystem->ScheduleJob(m_jobContext.m_done);
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
    
    jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
    mainSystem->ScheduleJob(new CreateFenceJob(ctx));
}

#undef THROW_ERROR
