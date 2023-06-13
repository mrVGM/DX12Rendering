#include "ResourceUtils/DXCopyBuffers.h"

#include "ResourceUtils/DXCopyBuffersMeta.h"
#include "ResourceUtils/DXCopyFenceMeta.h"

#include "DXBuffer.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"
#include "Job.h"
#include "CopyJobSystemMeta.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXDevice* m_device = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        if (!m_device)
        {
            m_device = core::utils::GetDevice();
        }
    }

    struct CopyCommandList 
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

        CopyCommandList()
        {
            using Microsoft::WRL::ComPtr;
            using namespace rendering;

            THROW_ERROR(
                m_device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
                "Can't create Command Allocator!")

            THROW_ERROR(
                m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
                "Can't create Command List!")

            THROW_ERROR(
                m_commandList->Close(),
                "Can't close command List!")
        }

        void Execute(
            rendering::DXBuffer& dst,
            const rendering::DXBuffer& src,
            rendering::DXCopyCommandQueue* commandQueue,
            ID3D12Fence* fence,
            UINT64 signal)
        {
            using namespace rendering;

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


            ID3D12CommandList* copyCommandList[] = { m_commandList.Get() };
            commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

            commandQueue->GetCommandQueue()->Signal(fence, signal);
        }
    };
}


rendering::DXCopyBuffers::DXCopyBuffers() :
    BaseObject(DXCopyBuffersMeta::GetInstance())
{
    CacheObjects();

    m_copyCommandQueue = core::utils::GetCopyCommandQueue();

    m_copyJobSytem = new jobs::JobSystem(CopyJobSystemMeta::GetInstance(), 1);
    m_copyFence = new DXFence(DXCopyFenceMeta::GetInstance());
}

rendering::DXCopyBuffers::~DXCopyBuffers()
{
}

void rendering::DXCopyBuffers::Execute(
    DXBuffer& dst,
    const DXBuffer& src,
    jobs::Job* done)
{
    struct JobContext
    {
        DXCopyBuffers* m_dxCopyBuffers = nullptr;
        DXBuffer* m_dst = nullptr;
        const DXBuffer* m_src = nullptr;
        CopyCommandList* m_copyCommandList = nullptr;
        UINT64 m_signal = -1;
        jobs::Job* m_done = nullptr;
    };

    JobContext ctx{ this, &dst, &src, new CopyCommandList(), -1, done };

    class WaitJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        WaitJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            WaitFence waitFence(*m_jobContext.m_dxCopyBuffers->m_copyFence);
            waitFence.Wait(m_jobContext.m_signal);

            core::utils::RunSync(m_jobContext.m_done);
            delete m_jobContext.m_copyCommandList;
        }
    };

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
            DXFence* fence = m_jobContext.m_dxCopyBuffers->m_copyFence;
            DXCopyCommandQueue* commandQueue = m_jobContext.m_dxCopyBuffers->m_copyCommandQueue;
            UINT64 signal = m_jobContext.m_dxCopyBuffers->m_copyCounter++;
            m_jobContext.m_signal = signal;

            m_jobContext.m_copyCommandList->Execute(*m_jobContext.m_dst, *m_jobContext.m_src, commandQueue, fence->GetFence(), signal);
            core::utils::RunAsync(new WaitJob(m_jobContext));
        }
    };
    
    m_copyJobSytem->ScheduleJob(new CopyJob(ctx));
}

void rendering::DXCopyBuffers::Execute(ID3D12CommandList* const* lists, UINT64 numLists, jobs::Job* done)
{
    struct JobContext
    {
        DXCopyBuffers* m_dxCopyBuffers = nullptr;
        ID3D12CommandList* const* m_lists = nullptr;
        UINT64 m_numLists = 0;
        UINT64 m_signal = -1;
        jobs::Job* m_done = nullptr;
    };

    JobContext ctx{ this, lists, numLists, -1, done };

    class WaitJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        WaitJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            WaitFence waitFence(*m_jobContext.m_dxCopyBuffers->m_copyFence);
            waitFence.Wait(m_jobContext.m_signal);

            core::utils::RunSync(m_jobContext.m_done);
        }
    };

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
            DXFence* fence = m_jobContext.m_dxCopyBuffers->m_copyFence;
            DXCopyCommandQueue* commandQueue = m_jobContext.m_dxCopyBuffers->m_copyCommandQueue;
            UINT64 signal = m_jobContext.m_dxCopyBuffers->m_copyCounter++;

            m_jobContext.m_signal = signal;
            core::utils::RunAsync(new WaitJob(m_jobContext));

            commandQueue->GetCommandQueue()->ExecuteCommandLists(m_jobContext.m_numLists, m_jobContext.m_lists);
            commandQueue->GetCommandQueue()->Signal(fence->GetFence(), signal);
        }
    };

    m_copyJobSytem->ScheduleJob(new CopyJob(ctx));
}

#undef THROW_ERROR
