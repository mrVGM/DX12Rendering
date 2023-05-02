#include "Rendering.h"

#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXSwapChain.h"
#include "DXRenderer.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"
#include "JobSystem.h"
#include "Job.h"

#include "BaseObjectContainer.h"
#include "RenderUtils.h"

#include <iostream>

void rendering::InitBaseObjects()
{
	new DXDevice();
	new DXCommandQueue();
	new DXSwapChain();
	DXRenderer* renderer = new DXRenderer();
	new jobs::JobSystem(LoadJobSystemMeta::GetInstance(), 5);
	new jobs::JobSystem(MainJobSystemMeta::GetInstance(), 1);

	rendering::utils::CacheObjects();
	std::cout << "Base Rendering Objects created!" << std::endl;

	jobs::JobSystem* mainJobSystem = rendering::utils::GetMainJobSystem();
	class StartExclusiveAccessJob : public jobs::Job
	{
	private:
		DXRenderer& m_renderer;
	public:
		StartExclusiveAccessJob(DXRenderer& renderer) :
			m_renderer(renderer)
		{
		}
		void Do()
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();
			container.StartExclusiveThreadAccess();
			m_renderer.StartRendering();
		}
	};

	mainJobSystem->ScheduleJob(new StartExclusiveAccessJob(*renderer));
}