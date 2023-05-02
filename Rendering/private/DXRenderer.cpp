#include "DXRenderer.h"

#include "DXRendererMeta.h"

#include "BaseObjectContainer.h"

#include "RenderUtils.h"

#include "JobSystem.h"
#include "RenderJobSystemMeta.h"
#include "DXFence.h"
#include "RenderFenceMeta.h"
#include "DXClearRTCL.h"
#include "DXClearRTCLMeta.h"
#include "WaitFence.h"

#include "Job.h"

#include <iostream>

namespace
{
	jobs::JobSystem* GetRenderJobSystem()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::RenderJobSystemMeta::GetInstance());
		if (!obj)
		{
			obj = new jobs::JobSystem(rendering::RenderJobSystemMeta::GetInstance(), 1);
		}

		return static_cast<jobs::JobSystem*>(obj);
	}

	rendering::DXFence* GetRenderFence()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::RenderFenceMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXFence(rendering::RenderFenceMeta::GetInstance());
		}

		return static_cast<rendering::DXFence*>(obj);
	}

	rendering::DXClearRTCL* GetClearRTCL()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXClearRTCLMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXClearRTCL();
		}

		return static_cast<rendering::DXClearRTCL*>(obj);
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
			std::string error;
			bool res = m_renderer.Render(error);
			if (!res)
			{
				std::cerr << error << std::endl;
				return;
			}

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

bool rendering::DXRenderer::Render(std::string& errorMessage)
{
	DXSwapChain* swapChain = utils::GetSwapChain();
	swapChain->UpdateCurrentFrameIndex();

	DXClearRTCL* clearRT = GetClearRTCL();

	std::string error;
	bool res = clearRT->Populate(error);
	if (!res)
	{
		errorMessage = error;
		return false;
	}

	DXFence* fence = GetRenderFence();
	WaitFence waitFence(*fence);

	res = clearRT->Execute(error);
	if (!res)
	{
		errorMessage = error;
		return false;
	}
	DXCommandQueue* commandQueue = utils::GetCommandQueue();
	commandQueue->GetCommandQueue()->Signal(fence->GetFence(), m_counter);

	res = waitFence.Wait(m_counter, error);
	if (!res)
	{
		errorMessage = error;
		return false;
	}

	res = swapChain->Present(error);
	if (!res)
	{
		errorMessage = error;
		return false;
	}

	++m_counter;
	return true;
}

void rendering::DXRenderer::RenderFrame()
{
	jobs::JobSystem* renderJobSystem = GetRenderJobSystem();
	renderJobSystem->ScheduleJob(new RenderJob(*this, m_counter));
}