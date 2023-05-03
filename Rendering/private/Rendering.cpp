#include "Rendering.h"

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "DXSwapChain.h"
#include "DXRenderer.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"
#include "DXCameraBufferMeta.h"
#include "JobSystem.h"
#include "Job.h"

#include "DXScene.h"

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "DataLib.h"

#include <iostream>

void rendering::InitBaseObjects()
{
	new rendering::Window();
	new DXDevice();
	new DXCommandQueue();
	new DXCopyCommandQueue();
	new DXSwapChain();
	DXRenderer* renderer = new DXRenderer();
	new jobs::JobSystem(LoadJobSystemMeta::GetInstance(), 5);
	new jobs::JobSystem(MainJobSystemMeta::GetInstance(), 1);

	new rendering::DXCamera();
	new rendering::DXBuffer(DXCameraBufferMeta::GetInstance());

	new DXScene();

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

	class InitCamBuffJob : public jobs::Job
	{
	private:
		DXRenderer& m_renderer;
	public:
		InitCamBuffJob(DXRenderer& renderer) :
			m_renderer(renderer)
		{
		}
		void Do() override
		{
			jobs::JobSystem* mainJobSystem = utils::GetMainJobSystem();
			mainJobSystem->ScheduleJob(new StartExclusiveAccessJob(m_renderer));
		}
	};

	DXCamera* cam = utils::GetCamera();
	cam->InitBuffer(new InitCamBuffJob(*renderer));


	class LoadColladaSceneJob : public jobs::Job
	{
	private:
	public:
		LoadColladaSceneJob()
		{
		}
		void Do() override
		{
			bool t = true;
		}
	};

	DXScene* scene = utils::GetScene();

	std::string cubePath = data::GetLibrary().GetRootDir() + "geo/cube.dae";
	scene->LoadColladaScene(cubePath, new LoadColladaSceneJob());
}