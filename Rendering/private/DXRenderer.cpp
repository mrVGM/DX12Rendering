#include "DXRenderer.h"

#include "DXRendererMeta.h"

#include "BaseObjectContainer.h"

#include "RenderUtils.h"

#include "JobSystem.h"
#include "DXFence.h"
#include "RenderFenceMeta.h"
#include "RenderPass/DXClearRTRP.h"
#include "RenderPass/DXClearRTRPMeta.h"
#include "RenderPass/DXUnlitRP.h"
#include "RenderPass/DXUnlitRPMeta.h"
#include "RenderPass/DXClearDSTRP.h"
#include "RenderPass/DXClearDSTRPMeta.h"
#include "RenderPass/DXDeferredRP.h"
#include "RenderPass/DXDeferredRPMeta.h"

#include "RenderPass/DXDisplaySMRP.h"
#include "RenderPass/DXDisplaySMRPMeta.h"

#include "WaitFence.h"

#include "Job.h"

#include <vector>

#include <iostream>

namespace
{
	rendering::DXFence* m_renderFence = nullptr;
	rendering::DXClearRTRP* m_clearRTRP = nullptr;
	rendering::DXClearDSTRP* m_clearDSTRP = nullptr;
	rendering::DXUnlitRP* m_unlitRP = nullptr;
	rendering::DXDeferredRP* m_deferredRP = nullptr;
	rendering::DXDisplaySMRP* m_displaySMPR = nullptr;


	rendering::DXFence* GetRenderFence()
	{
		if (m_renderFence)
		{
			return m_renderFence;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::RenderFenceMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXFence(rendering::RenderFenceMeta::GetInstance());
		}

		m_renderFence = static_cast<rendering::DXFence*>(obj);
		return m_renderFence;
	}

	rendering::DXClearRTRP* GetClearRTRP()
	{
		if (m_clearRTRP)
		{
			return m_clearRTRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXClearRTRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXClearRTRP();
		}

		m_clearRTRP = static_cast<rendering::DXClearRTRP*>(obj);
		return m_clearRTRP;
	}

	rendering::DXUnlitRP* GetUnlitRP()
	{
		if (m_unlitRP)
		{
			return m_unlitRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXUnlitRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXUnlitRP();
		}

		m_unlitRP = static_cast<rendering::DXUnlitRP*>(obj);
		return m_unlitRP;
	}

	rendering::DXClearDSTRP* GetClearDSTRP()
	{
		if (m_clearDSTRP)
		{
			return m_clearDSTRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXClearDSTRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXClearDSTRP();
		}

		m_clearDSTRP = static_cast<rendering::DXClearDSTRP*>(obj);
		return m_clearDSTRP;
	}

	rendering::DXDeferredRP* GetDeferredRP()
	{
		if (m_deferredRP)
		{
			return m_deferredRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXDeferredRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXDeferredRP();
		}

		m_deferredRP = static_cast<rendering::DXDeferredRP*>(obj);
		return m_deferredRP;
	}

	rendering::DXDisplaySMRP* GetDisplaySMPR()
	{
		if (m_displaySMPR)
		{
			return m_displaySMPR;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXDisplaySMRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXDisplaySMRP();
		}

		m_displaySMPR = static_cast<rendering::DXDisplaySMRP*>(obj);
		return m_displaySMPR;
	}
}

rendering::DXRenderer::DXRenderer() :
	BaseObject(DXRendererMeta::GetInstance())
{
}

rendering::DXRenderer::~DXRenderer()
{
}

void rendering::DXRenderer::Render(jobs::Job* done)
{
	DXSwapChain* swapChain = utils::GetSwapChain();
	swapChain->UpdateCurrentFrameIndex();

	DXClearRTRP* clearRT = GetClearRTRP();
	DXClearDSTRP* clearDST = GetClearDSTRP();
	DXUnlitRP* unlitRP = GetUnlitRP();
	DXDeferredRP* deferredRP = GetDeferredRP();
	DXDisplaySMRP* displaySMPR = GetDisplaySMPR();

	clearRT->Prepare();
	clearDST->Prepare();
	deferredRP->Prepare();
	unlitRP->Prepare();
	displaySMPR->Prepare();

	DXFence* fence = GetRenderFence();
	WaitFence waitFence(*fence);

	clearRT->Execute();
	clearDST->Execute();
	deferredRP->Execute();
	unlitRP->Execute();
	displaySMPR->Execute();

	DXCommandQueue* commandQueue = utils::GetCommandQueue();
	commandQueue->GetCommandQueue()->Signal(fence->GetFence(), m_counter);

	waitFence.Wait(m_counter);

	swapChain->Present();

	++m_counter;

	utils::RunSync(done);
}

void rendering::DXRenderer::RenderFrame(jobs::Job* done)
{
	struct Context
	{
		DXRenderer* m_renderer = nullptr;
		jobs::Job* m_done;
	};

	class RenderJob : public jobs::Job
	{
	private:
		Context m_ctx;
		
	public:
		RenderJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_renderer->Render(m_ctx.m_done);
		}
	};

	Context ctx {this, done};
	utils::RunAsync(new RenderJob(ctx));
}


void rendering::DXRenderer::LoadRPs(jobs::Job* done)
{
	struct Context
	{
		int m_jobsInProgress = 5;
		jobs::Job* m_done = nullptr;
	};

	class LoadReady : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadReady(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_jobsInProgress;

			if (m_ctx.m_jobsInProgress > 0)
			{
				return;
			}

			utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	class CreateObjects : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		CreateObjects(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			GetRenderFence();
			{
				RenderPass* rp = GetClearRTRP();
				rp->Load(new LoadReady(m_ctx));
			}

			{
				RenderPass* rp = GetClearDSTRP();
				rp->Load(new LoadReady(m_ctx));
			}
			
			{
				RenderPass* rp = GetUnlitRP();
				rp->Load(new LoadReady(m_ctx));
			}
			
			{
				RenderPass* rp = GetDeferredRP();
				rp->Load(new LoadReady(m_ctx));
			}

			{
				RenderPass* rp = GetDisplaySMPR();
				rp->Load(new LoadReady(m_ctx));
			}
		}
	};

	Context* ctx = new Context();
	ctx->m_done = done;

	utils::RunSync(new CreateObjects(*ctx));
}