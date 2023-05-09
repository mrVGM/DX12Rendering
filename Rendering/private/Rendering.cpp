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

#include <iostream>

namespace
{
	struct BootContext
	{
		bool m_camBufferLoaded = false;
		bool m_errorMatLoaded = false;
		bool m_depthStencilTextureLoaded = false;
		bool m_gBufferReady = false;
	};

	void LoadErrorMaterial(jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
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
				rendering::shader_repo::LoadShaderPrograms();

				DXShader* ps = rendering::shader_repo::GetErrorPixelShader();
				DXShader* vs = rendering::shader_repo::GetMainVertexShader();

				new DXUnlitErrorMaterial(*vs, *ps);
				DXMaterial* errorMat = utils::GetUnlitErrorMaterial();

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("error", *errorMat);

				utils::RunSync(m_ctx.m_done);
			}
		};

		Context ctx{ done };

		utils::RunSync(new CreateShadersAndMaterial(ctx));
	}

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

				DXDescriptorHeap::CreateDSVDescriptorHeap(*dsTex);
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
				DXTexture::CreateDepthStencilTexture(wnd->m_width, wnd->m_height);
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

	void LoadDeferredMaterial()
	{
		using namespace rendering;

		struct Context
		{
			DXDeferredMaterial* m_material = nullptr;
		};

		class SettingsBufferReady : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			SettingsBufferReady(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				DXBuffer* buffer = m_ctx.m_material->GetSettingsBuffer();
				float color[] = { 1, 1, 0, 1 };
				buffer->CopyData(color, _countof(color) * sizeof(float));

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("yellow", *m_ctx.m_material);
			}
		};

		class CreateDeferredMaterial : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateDeferredMaterial(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				m_ctx.m_material = new DXDeferredMaterial(*shader_repo::GetMainVertexShader(), *shader_repo::GetDeferredPixelShader());
				m_ctx.m_material->CreateSettingsBuffer(new SettingsBufferReady(m_ctx));
			}
		};

		Context ctx;
		utils::RunSync(new CreateDeferredMaterial(ctx));
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

				if (!m_ctx.m_errorMatLoaded)
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

				LoadDeferredMaterial();

				Updater* updater = utils::GetUpdater();
				updater->Start();
			}
		};

		BootContext* ctx = new BootContext();
		LoadCamAndBuffer(new ItemReady(*ctx, ctx->m_camBufferLoaded));
		LoadErrorMaterial(new ItemReady(*ctx, ctx->m_errorMatLoaded));
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

#if true
				{
					collada::ColladaScene* tmpScene = scene->m_colladaScenes[scene->m_scenesLoaded];
					std::string& matOverride = *(tmpScene->GetScene().m_objects.begin()->second.m_materialOverrides.begin());
					matOverride = "yellow";
				}
#endif

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

	void LoadCyanMaterial()
	{
		using namespace rendering;

		struct Context
		{
			DXUnlitMaterial* m_material = nullptr;
		};

		class SettingsBufferReady : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			SettingsBufferReady(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				DXBuffer* buffer = m_ctx.m_material->GetSettingsBuffer();
				float color[] = { 0, 1, 1, 1 };
				buffer->CopyData(color, _countof(color) * sizeof(float));

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("cyan", *m_ctx.m_material);
			}
		};

		class CreateUnlitMaterial : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateUnlitMaterial(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				m_ctx.m_material = new DXUnlitMaterial(*shader_repo::GetMainVertexShader(), *shader_repo::GetUnlitPixelShader());
				m_ctx.m_material->CreateSettingsBuffer(new SettingsBufferReady(m_ctx));
			}
		};

		Context ctx;
		utils::RunSync(new CreateUnlitMaterial(ctx));
	}	
}

void rendering::Boot()
{
	InitBaseObjects();

	LoadScene();
	LoadRenderPipepine();
	LoadCyanMaterial();
}