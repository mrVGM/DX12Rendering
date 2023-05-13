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

#include "DXHeap.h"

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

#include "Materials/SharederRepo.h"
#include "Materials/DXUnlitMaterial.h"
#include "Materials/DXDeferredMaterial.h"

#include "Deferred/DeferredRendering.h"

#include "DXDepthStencilTextureMeta.h"
#include "DXDepthStencilDescriptorHeapMeta.h"

#include <iostream>

namespace
{
	struct BootContext
	{
		bool m_camBufferLoaded = false;
		bool m_depthStencilTextureLoaded = false;
		bool m_gBufferReady = false;
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
				new DXBuffer(DXCameraBufferMeta::GetInstance());

				DXCamera* cam = utils::GetCamera();
				utils::GetCameraBuffer();

				cam->InitBuffer(m_ctx.m_done);
			}
		};

		Context ctx{ done };

		utils::RunSync(new CreateCamAndBuffer(ctx));
	}

	void LoadDepthStencilTexture(jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			DXHeap* m_heap = nullptr;
			jobs::Job* m_done = nullptr;
		};

		class DSTexReady : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			DSTexReady(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				DXTexture* dsTex = utils::GetDepthStencilTexture();
				dsTex->Place(*m_ctx.m_heap, 0);

				DXDescriptorHeap::CreateDSVDescriptorHeap(DXDepthStencilDescriptorHeapMeta::GetInstance(), *dsTex);
				utils::GetDSVDescriptorHeap();

				utils::RunSync(m_ctx.m_done);
			}
		};

		class CreateDSTex : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateDSTex(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				Window* wnd = utils::GetWindow();
				DXTexture::CreateDepthStencilTexture(DXDepthStencilTextureMeta::GetInstance(), wnd->m_width, wnd->m_height);
				DXTexture* dsTex = utils::GetDepthStencilTexture();

				D3D12_RESOURCE_ALLOCATION_INFO allocInfo = dsTex->GetTextureAllocationInfo();

				DXHeap* heap = new DXHeap();
				m_ctx.m_heap = heap;

				heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				heap->SetHeapSize(allocInfo.SizeInBytes);

				heap->Create();
				heap->MakeResident(new DSTexReady(m_ctx));
			}
		};

		Context ctx{ nullptr, done };

		utils::RunSync(new CreateDSTex(ctx));
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

				if (!m_ctx.m_depthStencilTextureLoaded)
				{
					return;
				}
				
				if (!m_ctx.m_gBufferReady)
				{
					return;
				}

				delete &m_ctx;

				Updater* updater = utils::GetUpdater();
				updater->Start();

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->EnableMaterialLoading();
			}
		};

		BootContext* ctx = new BootContext();
		LoadCamAndBuffer(new ItemReady(*ctx, ctx->m_camBufferLoaded));
		LoadDepthStencilTexture(new ItemReady(*ctx, ctx->m_depthStencilTextureLoaded));
		deferred::LoadGBuffer(new ItemReady(*ctx, ctx->m_gBufferReady));
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
				int sceneIndex = scene->m_scenesLoaded;
				++scene->m_scenesLoaded;

				DXMaterialRepo* repo = utils::GetMaterialRepo();

				const collada::Scene& justLoaded = scene->m_colladaScenes[sceneIndex]->GetScene();
				for (auto it = justLoaded.m_materials.begin(); it != justLoaded.m_materials.end(); ++it)
				{
					const collada::ColladaMaterial& mat = it->second;
					repo->LoadColladaMaterial(mat);
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
		new DXMaterialRepo();

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