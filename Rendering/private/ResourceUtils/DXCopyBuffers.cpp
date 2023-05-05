#include "ResourceUtils/DXCopyBuffers.h"

#include "ResourceUtils/DXCopyBuffersMeta.h"
#include "ResourceUtils/DXCopyFenceMeta.h"

#include "DXBuffer.h"
#include "DXFence.h"
#include "DXFenceMeta.h"
#include "WaitFence.h"
#include "Job.h"

#include "RenderUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    struct CopyCommandList 
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

        CopyCommandList()
        {
            using Microsoft::WRL::ComPtr;
            using namespace rendering;

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

        void Execute(
            rendering::DXBuffer& dst,
            const rendering::DXBuffer& src,
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


            DXCopyCommandQueue* commandQueue = rendering::utils::GetCopyCommandQueue();
            ID3D12CommandList* copyCommandList[] = { m_commandList.Get() };
            commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

            commandQueue->GetCommandQueue()->Signal(fence, signal);
        }
    };
}


rendering::DXCopyBuffers::DXCopyBuffers() :
    BaseObject(DXCopyBuffersMeta::GetInstance())
{
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

    class Clear : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        Clear(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }
        void Do()
        {
            delete m_jobContext.m_copyCommandList;
        }
    };

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

            utils::RunSync(m_jobContext.m_done);
            utils::RunSync(new Clear(m_jobContext));
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
            UINT64 signal = m_jobContext.m_dxCopyBuffers->m_copyCounter++;
            m_jobContext.m_signal = signal;

            m_jobContext.m_copyCommandList->Execute(*m_jobContext.m_dst, *m_jobContext.m_src, fence->GetFence(), signal);
            utils::RunAsync(new WaitJob(m_jobContext));
        }
    };
    
    utils::RunSync(new CopyJob(ctx));
}

#undef THROW_ERROR
