#include "DXRenderer.h"

#include "DXRendererMeta.h"

#include "BaseObjectContainer.h"

#include "RenderUtils.h"

#include "JobSystem.h"
#include "DXFence.h"
#include "RenderFenceMeta.h"
#include "DXClearRTRP.h"
#include "DXClearRTRPMeta.h"
#include "DXUnlitRP.h"
#include "DXUnlitRPMeta.h"
#include "DXClearDSTRP.h"
#include "DXClearDSTRPMeta.h"
#include "DXDeferredRP.h"
#include "DXDeferredRPMeta.h"
#include "DXOverlayRP.h"
#include "DXOverlayRPMeta.h"
#include "DXPostProcessRP.h"
#include "DXPostProcessRPMeta.h"


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
	rendering::DXPostProcessRP* m_postProcessRP = nullptr;
	rendering::overlay::DXOverlayRP* m_overlayRP = nullptr;


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

	rendering::overlay::DXOverlayRP* GetOverlayRP()
	{
		if (m_overlayRP)
		{
			return m_overlayRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::overlay::DXOverlayRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::overlay::DXOverlayRP();
		}

		m_overlayRP = static_cast<rendering::overlay::DXOverlayRP*>(obj);
		return m_overlayRP;
	}

	rendering::DXPostProcessRP* GetPostProcessRP()
	{
		if (m_postProcessRP)
		{
			return m_postProcessRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXPostProcessRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXPostProcessRP();
		}

		m_postProcessRP = static_cast<rendering::DXPostProcessRP*>(obj);
		return m_postProcessRP;
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
	DXPostProcessRP* postProcessRP = GetPostProcessRP();
	overlay::DXOverlayRP* overlayRP = GetOverlayRP();

	clearRT->Prepare();
	clearDST->Prepare();
	deferredRP->Prepare();
	unlitRP->Prepare();
	postProcessRP->Prepare();
	overlayRP->Prepare();

	DXFence* fence = GetRenderFence();
	WaitFence waitFence(*fence);

	clearRT->Execute();
	clearDST->Execute();
	deferredRP->Execute();
	unlitRP->Execute();
	postProcessRP->Execute();
	overlayRP->Execute();

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
		int m_jobsInProgress = 6;
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
				RenderPass* rp = GetPostProcessRP();
				rp->Load(new LoadReady(m_ctx));
			}

			{
				RenderPass* rp = GetOverlayRP();
				rp->Load(new LoadReady(m_ctx));
			}

		}
	};

	Context* ctx = new Context();
	ctx->m_done = done;

	utils::RunSync(new CreateObjects(*ctx));
}