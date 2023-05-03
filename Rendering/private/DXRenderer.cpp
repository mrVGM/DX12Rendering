#include "DXRenderer.h"

#include "DXRendererMeta.h"

#include "BaseObjectContainer.h"

#include "RenderUtils.h"

#include "JobSystem.h"
#include "RenderJobSystemMeta.h"
#include "DXFence.h"
#include "RenderFenceMeta.h"
#include "RenderPass/DXClearRTRP.h"
#include "RenderPass/DXClearRTRPMeta.h"
#include "WaitFence.h"

#include "Job.h"

#include <iostream>

namespace
{
	rendering::DXFence* m_renderFence = nullptr;
	jobs::JobSystem* m_renderJobSystem = nullptr;
	rendering::DXClearRTRP* m_clearRTRP = nullptr;

	jobs::JobSystem* GetRenderJobSystem()
	{
		if (m_renderJobSystem)
		{
			return m_renderJobSystem;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::RenderJobSystemMeta::GetInstance());
		if (!obj)
		{
			obj = new jobs::JobSystem(rendering::RenderJobSystemMeta::GetInstance(), 1);
		}

		m_renderJobSystem = static_cast<jobs::JobSystem*>(obj);
		return m_renderJobSystem;
	}

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

	class RenderJob : public jobs::Job
	{
	private:
		rendering::DXRenderer& m_renderer;
	public:
		RenderJob(rendering::DXRenderer& renderer, UINT64 signal) :
			m_renderer(renderer)
		{
		}

		void Do() override
		{	
			m_renderer.Render();

			m_renderer.RenderFrame();
		}

		virtual ~RenderJob()
		{
			bool t = true;
		}
	};
}


rendering::DXRenderer::DXRenderer() :
	BaseObject(DXRendererMeta::GetInstance())
{
}

rendering::DXRenderer::~DXRenderer()
{
}

void rendering::DXRenderer::Render()
{
	DXSwapChain* swapChain = utils::GetSwapChain();
	swapChain->UpdateCurrentFrameIndex();

	DXClearRTRP* clearRT = GetClearRTRP();

	clearRT->Prepare();

	DXFence* fence = GetRenderFence();
	WaitFence waitFence(*fence);

	clearRT->Execute();
	
	DXCommandQueue* commandQueue = utils::GetCommandQueue();
	commandQueue->GetCommandQueue()->Signal(fence->GetFence(), m_counter);

	waitFence.Wait(m_counter);

	swapChain->Present();

	++m_counter;
}

void rendering::DXRenderer::RenderFrame()
{
	jobs::JobSystem* renderJobSystem = GetRenderJobSystem();
	renderJobSystem->ScheduleJob(new RenderJob(*this, m_counter));
}

void rendering::DXRenderer::StartRendering()
{
	class StartRenderJob : public jobs::Job
	{
	private:
		DXRenderer& m_renderer;
	public:
		StartRenderJob(DXRenderer& renderer) :
			m_renderer(renderer)
		{
		}
		void Do()
		{
			GetRenderJobSystem();
			GetRenderFence();
			GetClearRTRP();

			m_renderer.RenderFrame();
		}
	};

	rendering::utils::GetMainJobSystem()->ScheduleJob(new StartRenderJob(*this));
}