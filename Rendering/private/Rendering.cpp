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

#include "ResourceUtils/DXCopyBuffers.h"

#include "DXScene.h"

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "DXShader.h"
#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"


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

	new DXCopyBuffers();
	new DXScene();

	DXShader* ps = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_unlit.fxc");
	DXShader* vs = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_unlit.fxc");

	new DXUnlitMaterial(*vs, *ps);

	rendering::utils::CacheObjects();
	std::cout << "Base Rendering Objects created!" << std::endl;

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
			utils::RunSync(new StartExclusiveAccessJob(m_renderer));
		}
	};

	DXCamera* cam = utils::GetCamera();
	cam->InitBuffer(new InitCamBuffJob(*renderer));


	DXScene* scene = utils::GetScene();
	struct CTX
	{
		rendering::DXScene* scene = nullptr;
	};

	CTX ctx{ scene };

	class SceneLoaded : public jobs::Job
	{
	private:
		CTX m_ctx;
	public:
		SceneLoaded(const CTX& ctx) :
			m_ctx(ctx)
		{
		}
		void Do() override
		{
			std::cout << "Yessss!" << std::endl;
			bool t = true;
		}
	};

	std::string cubePath = data::GetLibrary().GetRootDir() + "geo/cube.dae";
	scene->LoadColladaScene(cubePath, new SceneLoaded(ctx));
}