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
#include "Updater.h"

#include "DXMaterialRepo.h"

#include "DataLib.h"

#include <iostream>

namespace
{
	struct BootContext
	{
		bool m_camBufferLoaded = false;
		bool m_errorMatLoaded = false;
	};

	void LoadErrorMaterial(BootContext& bootContext, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			BootContext* m_bootCtx = nullptr;
			jobs::Job* m_done = nullptr;
		};

		class CreateShadersAndMaterial : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateShadersAndMaterial(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				DXShader* ps = new DXShader(DXPixelShaderMeta::GetInstance(), "shaders/bin/ps_error.fxc");
				DXShader* vs = new DXShader(DXVertexShaderMeta::GetInstance(), "shaders/bin/vs_mainVS.fxc");

				new DXUnlitErrorMaterial(*vs, *ps);
				DXMaterial* errorMat = utils::GetUnlitErrorMaterial();

				new DXMaterialRepo();
				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("error", *errorMat);

				m_ctx.m_bootCtx->m_errorMatLoaded = true;
				utils::RunSync(m_ctx.m_done);
			}
		};

		Context ctx{ &bootContext, done };

		utils::RunSync(new CreateShadersAndMaterial(ctx));
	}


	void LoadCamAndBuffer(BootContext& bootContext, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			BootContext* m_bootCtx = nullptr;
			jobs::Job* m_done = nullptr;
		};

		class CamBufferDone : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CamBufferDone(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				m_ctx.m_bootCtx->m_camBufferLoaded = true;
				utils::RunSync(m_ctx.m_done);
			}
		};

		class CreateCamAndBuffer : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateCamAndBuffer(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				new DXCamera();
				new DXBuffer(DXCameraBufferMeta::GetInstance());

				DXCamera* cam = utils::GetCamera();
				utils::GetCameraBuffer();

				cam->InitBuffer(new CamBufferDone(m_ctx));
			}
		};

		Context ctx{ &bootContext, done };

		utils::RunSync(new CreateCamAndBuffer(ctx));
	}

	void LoadRenderPipepine()
	{
		using namespace rendering;

		class ItemReady : public jobs::Job
		{
		private:
			BootContext& m_ctx;
		public:
			ItemReady(BootContext& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				if (!m_ctx.m_camBufferLoaded)
				{
					return;
				}

				if (!m_ctx.m_errorMatLoaded)
				{
					return;
				}

				delete &m_ctx;

				Updater* updater = utils::GetUpdater();
				updater->Start();
			}
		};

		BootContext* ctx = new BootContext();
		LoadCamAndBuffer(*ctx, new ItemReady(*ctx));
		LoadErrorMaterial(*ctx, new ItemReady(*ctx));
	}


	void LoadScene()
	{
		using namespace rendering;

		class SceneLoaded : public jobs::Job
		{
		private:
		public:
			SceneLoaded()
			{
			}

			void Do() override
			{
				DXScene* scene = utils::GetScene();
				++scene->m_scenesLoaded;
			}
		};

		class CreateDXScene : public jobs::Job
		{
		private:
		public:
			CreateDXScene()
			{
			}
			void Do() override
			{
				new DXScene();
				DXScene* scene = utils::GetScene();

				std::string cubePath = data::GetLibrary().GetRootDir() + "geo/cube.dae";
				scene->LoadColladaScene(cubePath, new SceneLoaded());
			}
		};

		utils::RunSync(new CreateDXScene());
	}


	void InitBaseObjects()
	{
		using namespace rendering;

		new jobs::JobSystem(LoadJobSystemMeta::GetInstance(), 5);
		new jobs::JobSystem(MainJobSystemMeta::GetInstance(), 1);

		new rendering::Window();
		new DXDevice();
		new DXCommandQueue();
		new DXCopyCommandQueue();
		new DXSwapChain();
		new DXRenderer();
		new DXCopyBuffers();
		new Updater();

		rendering::utils::CacheObjects();
		std::cout << "Base Rendering Objects created!" << std::endl;

		class StartExclusiveAccessJob : public jobs::Job
		{
		private:
		public:
			StartExclusiveAccessJob()
			{
			}
			void Do()
			{
				BaseObjectContainer& container = BaseObjectContainer::GetInstance();
				container.StartExclusiveThreadAccess();
			}
		};

		utils::RunSync(new StartExclusiveAccessJob());
	}
}

void rendering::Boot()
{
	InitBaseObjects();

	LoadScene();
	LoadRenderPipepine();
}