#include "DXScene.h"

#include "DXSceneMeta.h"

#include "CoreUtils.h"

#include "Job.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"
#include "DXHeap.h"

#include <list>

namespace
{
	void CreateBuffer(bool upload, UINT64 size, UINT64 stride, rendering::DXBuffer*& outBuffer, jobs::Job* done)
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

				core::utils::RunSync(m_ctx.m_done);
				delete& m_ctx;
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

				m_ctx.m_heap->Create();
				m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx));
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

		core::utils::RunSync(new Create(*ctx));
	}

	template<typename T>
	void UploadDataToBuffer(const std::list<T>& data, rendering::DXBuffer& buffer, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			DXBuffer* m_buffer = nullptr;
			const std::list<T>* m_data;
			jobs::Job* m_done = nullptr;
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

				T* cur = static_cast<T*>(dst);
				for (auto it = m_ctx.m_data->begin(); it != m_ctx.m_data->end(); ++it)
				{
					*cur = *it;
					++cur;
				}

				m_ctx.m_buffer->Unmap();
				core::utils::RunSync(m_ctx.m_done);
			}
		};

		Context ctx;
		ctx.m_buffer = &buffer;
		ctx.m_data = &data;
		ctx.m_done = done;

		core::utils::RunAsync(new UploadData(ctx));
	}

	template<typename T>
	void CreateDefaultBuffer(const std::list<T>& data, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			const std::list<T>* m_data;
			DXBuffer** m_buffer = nullptr;
			DXBuffer* m_uploadBuffer = nullptr;
			jobs::Job* m_done = nullptr;

			bool m_uploadBufferReady = false;
			bool m_defaultBufferReady = false;
		};

		UINT64 stride = sizeof(T);
		UINT64 size = data.size() * stride;

		Context* ctx = new Context();
		{
			ctx->m_data = &data;
			ctx->m_buffer = &buffer;
			*(ctx->m_buffer) = nullptr;

			ctx->m_uploadBuffer = nullptr;

			ctx->m_done = done;
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
				core::utils::RunSync(m_ctx.m_done);

				DXHeap* heap = m_ctx.m_uploadBuffer->GetResidentHeap();
				core::utils::DisposeBaseObject(*m_ctx.m_uploadBuffer);
				core::utils::DisposeBaseObject(*heap);
				delete &m_ctx;
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

				m_ctx.m_uploadBuffer->CopyBuffer(**(m_ctx.m_buffer), new Clear(m_ctx));
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
				core::utils::RunSync(new ExecuteCopy(m_ctx));
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
				UploadDataToBuffer<T>(*m_ctx.m_data, *m_ctx.m_uploadBuffer, new UploadBufferReady(m_ctx));
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
				core::utils::RunSync(new ExecuteCopy(m_ctx));
			}
		};

		CreateBuffer(true, size, stride, ctx->m_uploadBuffer, new CreateUploadBuffer(*ctx));
		CreateBuffer(false, size, stride, *(ctx->m_buffer), new CreateDefaultBuffer(*ctx));
	}

	template<typename T>
	void CreateCPUBuffer(const std::list<T>& data, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			const std::list<T>* m_data;
			DXBuffer** m_buffer = nullptr;
			DXBuffer* m_uploadBuffer = nullptr;
			jobs::Job* m_done = nullptr;
		};

		UINT64 stride = sizeof(T);
		UINT64 size = data.size() * stride;

		Context ctx;
		{
			ctx.m_data = &data;
			ctx.m_buffer = &buffer;
			*(ctx.m_buffer) = nullptr;

			ctx.m_uploadBuffer = nullptr;
			ctx.m_done = done;
		}

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
				*m_ctx.m_buffer = m_ctx.m_uploadBuffer;
				core::utils::RunSync(m_ctx.m_done);
			}
		};

		class CreateUploadBuffer : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateUploadBuffer(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				UploadDataToBuffer<T>(*m_ctx.m_data, *m_ctx.m_uploadBuffer, new UploadBufferReady(m_ctx));
			}
		};

		CreateBuffer(true, size, stride, ctx->m_uploadBuffer, new CreateUploadBuffer(ctx));
	}

	void LoadSceneVertexBuffer(const std::string& name, const collada::Geometry& geometry, rendering::DXScene::SceneResources& resources, jobs::Job* done)
	{
		using namespace rendering;
		resources.m_vertexBuffers[name] = nullptr;
		DXBuffer*& buff = resources.m_vertexBuffers[name];
		CreateDefaultBuffer<collada::Vertex>(geometry.m_vertices, buff, done);
	}

	void LoadSceneIndexBuffer(const std::string& name, const collada::Geometry& geometry, rendering::DXScene::SceneResources& resources, jobs::Job* done)
	{
		using namespace rendering;
		resources.m_indexBuffers[name] = nullptr;
		DXBuffer*& buff = resources.m_indexBuffers[name];
		CreateDefaultBuffer<int>(geometry.m_indices, buff, done);
	}

	void LoadInstanceBuffer(const std::string& name, const collada::InstanceBuffer& instanceBuffer, rendering::DXScene::SceneResources& resources, jobs::Job* done)
	{
		using namespace rendering;
		resources.m_instanceBuffers[name] = nullptr;
		DXBuffer*& buff = resources.m_instanceBuffers[name];

		CreateDefaultBuffer<collada::GeometryInstanceData>(instanceBuffer.m_data, buff, done);
	}
}


rendering::DXScene::DXScene() :
	BaseObject(DXSceneMeta::GetInstance())
{
}

rendering::DXScene::~DXScene()
{
}

void rendering::DXScene::LoadColladaScene(const std::string& filePath, jobs::Job* done)
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
			int index = m_context.m_dxScene->m_colladaScenes.size();

			m_context.m_dxScene->m_colladaScenes.push_back(m_context.m_scene);
			m_context.m_dxScene->m_sceneResources.push_back(SceneResources());

			m_context.m_dxScene->LoadVertexBuffers(index, m_context.m_done);
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
			core::utils::RunSync(new PostLoadColladaSceneJob(m_context));
		}
	};

	JobContext ctx{ this, filePath, new collada::ColladaScene(), done };
	core::utils::RunAsync(new LoadColladaSceneJob(ctx));
}


void rendering::DXScene::LoadVertexBuffers(int sceneIndex, jobs::Job* done)
{
	struct Context
	{
		DXScene* m_scene = nullptr;
		int m_left = 0;
		jobs::Job* m_done = nullptr;
	};

	const collada::Scene& scene = m_colladaScenes[sceneIndex]->GetScene();

	Context* ctx = new Context();
	ctx->m_scene = this;
	ctx->m_left = 2 * scene.m_geometries.size() + scene.m_instanceBuffers.size();
	ctx->m_done = done;

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

			core::utils::RunSync(m_ctx.m_done);
			delete& m_ctx;
		}
	};

	for (auto it = scene.m_geometries.begin(); it != scene.m_geometries.end(); ++it)
	{
		LoadSceneVertexBuffer(it->first, it->second, m_sceneResources[sceneIndex], new Loaded(*ctx));
		LoadSceneIndexBuffer(it->first, it->second, m_sceneResources[sceneIndex], new Loaded(*ctx));
	}

	for (auto it = scene.m_instanceBuffers.begin(); it != scene.m_instanceBuffers.end(); ++it)
	{
		LoadInstanceBuffer(it->first, it->second, m_sceneResources[sceneIndex], new Loaded(*ctx));
	}
}