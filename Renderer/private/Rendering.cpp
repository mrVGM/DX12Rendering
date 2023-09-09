#include "Renderer.h"

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

#include "RenderingCore.h"

#include "DXHeap.h"

#include "DXScene.h"

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "DXShader.h"
#include "Updater.h"

#include "DXMaterialRepo.h"

#include "DataLib.h"

#include "ShaderRepo.h"
#include "DXUnlitMaterial.h"
#include "DXDeferredMaterial.h"

#include "DXDepthStencilTextureMeta.h"
#include "DXDepthStencilDescriptorHeapMeta.h"

#include "RendererSettings.h"
#include "SceneSettings.h"

#include "MaterialUtils.h"

#include "RendererEntryPoint.h"

#include "ImageLoading.h"

#include "DXAnimUpdater.h"

#include "Animator.h"

#include <iostream>

namespace
{
	rendering::RendererSettings* m_rendererSettings = nullptr;

	struct BootContext
	{
		bool m_camBufferLoaded = false;
		bool m_rendererReady = false;
	};

	void LoadCamAndBuffer(jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			jobs::Job* m_done = nullptr;
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
				new DXMutableBuffer(DXCameraBufferMeta::GetInstance(), 256, 256);

				DXCamera* cam = utils::GetCamera();
				utils::GetCameraBuffer();

				cam->InitBuffer(m_ctx.m_done);
			}
		};

		Context ctx{ done };

		utils::RunSync(new CreateCamAndBuffer(ctx));
	}

	void LoadRenderPipepine()
	{
		using namespace rendering;

		class ItemReady : public jobs::Job
		{
		private:
			BootContext& m_ctx;
			bool& m_readyItem;
		public:
			ItemReady(BootContext& ctx, bool& readyItem) :
				m_ctx(ctx),
				m_readyItem(readyItem)
			{
			}
			void Do() override
			{
				m_readyItem = true;
				if (!m_ctx.m_camBufferLoaded)
				{
					return;
				}

				if (!m_ctx.m_rendererReady)
				{
					return;
				}

				delete &m_ctx;

				Updater* updater = utils::GetUpdater();
				updater->Start();

				material_utils::EnableMaterialLoading();
			}
		};

		BootContext* ctx = new BootContext();
		LoadCamAndBuffer(new ItemReady(*ctx, ctx->m_camBufferLoaded));

		DXRenderer* renderer = utils::GetRenderer();
		renderer->LoadRPs(new ItemReady(*ctx, ctx->m_rendererReady));
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

				const collada::Scene& justLoaded = scene->m_colladaScenes.back()->GetScene();
				for (auto it = justLoaded.m_materials.begin(); it != justLoaded.m_materials.end(); ++it)
				{
					const collada::ColladaMaterial& mat = it->second;
					material_utils::LoadMaterial(mat);
				}

				for (auto it = justLoaded.m_skeletonPoseBuffers.begin(); it != justLoaded.m_skeletonPoseBuffers.end(); ++it)
				{
					new animation::Animator(it->first);
				}
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

				scene->LoadAnimation("anim");

				const rendering::RendererSettings::Settings& settings = m_rendererSettings->GetSettings();
				std::string scenePath = settings.m_sceneToLoad;
				scene->LoadColladaScene(scenePath, new SceneLoaded());

				new DXAnimUpdater();
			}
		};

		utils::RunSync(new CreateDXScene());
	}

	void InitBaseObjects()
	{
		using namespace rendering;

		core::Boot();

		new DXRenderer();
		new Updater();
		new DXMaterialRepo();

		image_loading::Boot();

		rendering::shader_repo::LoadShaderPrograms();

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
	m_rendererSettings = new RendererSettings();
	new collada::SceneSettings();

	InitBaseObjects();

	LoadScene();
	LoadRenderPipepine();
}

void rendering::RegisterLib()
{
	new RendererEntryPoint();
}