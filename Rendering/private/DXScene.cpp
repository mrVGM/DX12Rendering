#include "DXScene.h"

#include "DXSceneMeta.h"

#include "RenderUtils.h"

#include "Job.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"
#include "DXHeap.h"

#include <list>

namespace
{
	void CreateBuffer(bool upload, UINT64 size, UINT64 stride, rendering::DXBuffer*& outBuffer, jobs::Job* done, jobs::JobSystem* jobSystem)
	{
		using namespace rendering;
		struct Context
		{

			DXBuffer** m_outBuffer = nullptr;
			DXBuffer* m_buffer = nullptr;
			DXHeap* m_heap = nullptr;

			bool m_upload = false;
			UINT64 m_stride = 0;
			UINT64 m_size = 0;

			jobs::Job* m_done = nullptr;
			jobs::JobSystem* m_jobSystem = nullptr;
		};

		class PlaceBuffer : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			PlaceBuffer(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer->Place(m_ctx.m_heap, 0);

				*m_ctx.m_outBuffer = m_ctx.m_buffer;

				m_ctx.m_jobSystem->ScheduleJob(m_ctx.m_done);
				delete& m_ctx;
			}
		};

		class MakeResident : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			MakeResident(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_heap->Create();
				m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx), utils::GetLoadJobSystem());
			}
		};

		class Create : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			Create(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_heap = new DXHeap();

				m_ctx.m_buffer->SetBufferSizeAndFlags(m_ctx.m_size, D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_buffer->SetBufferStride(m_ctx.m_stride);

				m_ctx.m_heap->SetHeapSize(m_ctx.m_size);
				m_ctx.m_heap->SetHeapType(m_ctx.m_upload ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);

				utils::GetLoadJobSystem()->ScheduleJob(new MakeResident(m_ctx));
			}
		};

		Context* ctx = new Context();
		ctx->m_buffer = nullptr;
		ctx->m_outBuffer = &outBuffer;
		ctx->m_heap = nullptr;
		ctx->m_upload = upload;
		ctx->m_stride = stride;
		ctx->m_size = size;
		ctx->m_done = done;
		ctx->m_jobSystem = jobSystem;

		utils::GetMainJobSystem()->ScheduleJob(new Create(*ctx));
	}

	void UploadDataToBuffer(const std::list<collada::Vertex>& data, rendering::DXBuffer& buffer, jobs::Job* done, jobs::JobSystem* jobSystem)
	{
		using namespace rendering;
		struct Context
		{
			DXBuffer* m_buffer = nullptr;
			const std::list<collada::Vertex>* m_data;
			jobs::Job* m_done = nullptr;
			jobs::JobSystem* m_jobSystem = nullptr;
		};

		class UploadData : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			UploadData(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				void* dst = m_ctx.m_buffer->Map();

				collada::Vertex* cur = static_cast<collada::Vertex*>(dst);
				for (auto it = m_ctx.m_data->begin(); it != m_ctx.m_data->end(); ++it)
				{
					*cur = *it;
					++cur;
				}

				m_ctx.m_buffer->Unmap();

				m_ctx.m_jobSystem->ScheduleJob(m_ctx.m_done);
			}
		};

		Context ctx;
		ctx.m_buffer = &buffer;
		ctx.m_data = &data;
		ctx.m_done = done;
		ctx.m_jobSystem = jobSystem;

		utils::GetLoadJobSystem()->ScheduleJob(new UploadData(ctx));
	}

	void CreateVertexBuffer(const std::list<collada::Vertex>& data, rendering::DXBuffer*& buffer, jobs::Job* done, jobs::JobSystem* jobSystem)
	{
		using namespace rendering;
		struct Context
		{
			const std::list<collada::Vertex>* m_data;
			DXBuffer** m_buffer = nullptr;
			DXBuffer* m_uploadBuffer = nullptr;

			jobs::Job* m_done = nullptr;
			jobs::JobSystem* m_jobSystem = nullptr;

			bool m_uploadBufferReady = false;
			bool m_defaultBufferReady = false;
		};

		UINT64 stride = sizeof(collada::Vertex);
		UINT64 size = data.size() * stride;

		Context* ctx = new Context();
		{
			ctx->m_data = &data;
			ctx->m_buffer = &buffer;
			*(ctx->m_buffer) = nullptr;

			ctx->m_uploadBuffer = nullptr;

			ctx->m_done = done;
			ctx->m_jobSystem = jobSystem;
		}

		class Clear : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			Clear(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				DXHeap* heap = m_ctx.m_uploadBuffer->GetResidentHeap();
				delete m_ctx.m_uploadBuffer;
				delete heap;

				m_ctx.m_jobSystem->ScheduleJob(m_ctx.m_done);

				delete &m_ctx;
			}
		};

		class Copy : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			Copy(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadBuffer->CopyBuffer(**(m_ctx.m_buffer), new Clear(m_ctx), utils::GetMainJobSystem());
			}
		};

		class ExecuteCopy : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			ExecuteCopy(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				if (!m_ctx.m_defaultBufferReady)
				{
					return;
				}

				if (!m_ctx.m_uploadBufferReady)
				{
					return;
				}

				utils::GetLoadJobSystem()->ScheduleJob(new Copy(m_ctx));
			}
		};

		class UploadBufferReady : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadBufferReady(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadBufferReady = true;
				utils::GetMainJobSystem()->ScheduleJob(new ExecuteCopy(m_ctx));
			}
		};

		class CreateUploadBuffer : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateUploadBuffer(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				UploadDataToBuffer(*m_ctx.m_data, *m_ctx.m_uploadBuffer, new UploadBufferReady(m_ctx), utils::GetMainJobSystem());
			}
		};
		class CreateDefaultBuffer : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateDefaultBuffer(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_defaultBufferReady = true;
				utils::GetMainJobSystem()->ScheduleJob(new ExecuteCopy(m_ctx));
			}
		};

		CreateBuffer(true, size, stride, ctx->m_uploadBuffer, new CreateUploadBuffer(*ctx), utils::GetLoadJobSystem());
		CreateBuffer(false, size, stride, *(ctx->m_buffer), new CreateDefaultBuffer(*ctx), utils::GetMainJobSystem());
	}


	void LoadSceneBuffer(const std::string& name, const collada::Geometry& geometry, rendering::DXScene::SceneResources& resources, jobs::Job* done, jobs::JobSystem* jobSystem)
	{
		using namespace rendering;
		struct Context
		{
			std::string m_name;
			rendering::DXScene::SceneResources* m_resources = nullptr;
			const collada::Geometry* m_geo = nullptr;

			DXBuffer* m_buffer = nullptr;

			jobs::Job* m_done = nullptr;
			jobs::JobSystem* m_jobSystem = nullptr;
		};

		Context ctx;
		ctx.m_name = name;
		ctx.m_resources = &resources;
		ctx.m_geo = &geometry;
		ctx.m_done = done;
		ctx.m_jobSystem = jobSystem;

		class CreateVB : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateVB(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_resources->m_vertexBuffers[m_ctx.m_name] = nullptr;
				DXBuffer*& buff = m_ctx.m_resources->m_vertexBuffers[m_ctx.m_name];
				CreateVertexBuffer(m_ctx.m_geo->m_vertices, buff, m_ctx.m_done, m_ctx.m_jobSystem);
			}
		};

		utils::GetLoadJobSystem()->ScheduleJob(new CreateVB(ctx));
	}
}


rendering::DXScene::DXScene() :
	BaseObject(DXSceneMeta::GetInstance())
{
}

rendering::DXScene::~DXScene()
{
}

void rendering::DXScene::LoadColladaScene(const std::string& filePath, jobs::Job* done, jobs::JobSystem* jobSystem)
{
	struct JobContext
	{
		DXScene* m_dxScene = nullptr;
		std::string m_filePath;
		collada::ColladaScene* m_scene = nullptr;
		jobs::Job* m_done = nullptr;
		jobs::JobSystem* m_jobSystem = nullptr;
	};

	class PostLoadColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		PostLoadColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_dxScene->m_colladaScenes.push_back(m_context.m_scene);
			m_context.m_dxScene->m_sceneResources.push_back(SceneResources());

			m_context.m_jobSystem->ScheduleJob(m_context.m_done);
		}
	};

	class LoadColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		LoadColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_scene->Load(m_context.m_filePath);

			jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
			mainSystem->ScheduleJob(new PostLoadColladaSceneJob(m_context));
		}
	};

	class CreateColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		CreateColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_scene = new collada::ColladaScene();

			jobs::JobSystem* loadSystem = utils::GetLoadJobSystem();
			loadSystem->ScheduleJob(new LoadColladaSceneJob(m_context));
		}
	};

	JobContext ctx{ this, filePath, nullptr, done, jobSystem };

	jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
	mainSystem->ScheduleJob(new CreateColladaSceneJob(ctx));
}


void rendering::DXScene::LoadVertexBuffers(int sceneIndex, jobs::Job* done, jobs::JobSystem* jobSystem)
{
	struct Context
	{
		DXScene* m_scene = nullptr;
		int m_left = 0;
		jobs::Job* m_done = nullptr;
		jobs::JobSystem* m_jobSystem = nullptr;
	};

	const collada::Scene& scene = m_colladaScenes[sceneIndex]->GetScene();

	Context* ctx = new Context();
	ctx->m_scene = this;
	ctx->m_left = scene.m_geometries.size();
	ctx->m_done = done;
	ctx->m_jobSystem = jobSystem;

	class Loaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		Loaded(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_left;
			if (m_ctx.m_left > 0)
			{
				return;
			}

			m_ctx.m_jobSystem->ScheduleJob(m_ctx.m_done);
			delete& m_ctx;
		}
	};

	for (auto it = scene.m_geometries.begin(); it != scene.m_geometries.end(); ++it)
	{
		LoadSceneBuffer(it->first, it->second, m_sceneResources[sceneIndex], new Loaded(*ctx), utils::GetMainJobSystem());
	}
}