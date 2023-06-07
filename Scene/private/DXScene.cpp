#include "DXScene.h"

#include "DXSceneMeta.h"

#include "CoreUtils.h"

#include "Job.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"
#include "DXHeap.h"

#include "DXMutableBuffer.h"
#include "DXMutableBufferMeta.h"

#include <list>

#include "ColladaEntities.h"

namespace
{
	void LoadVertexBuffer(const collada::Geometry& geo, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		struct Context
		{
			const collada::Geometry* m_geo = nullptr;

			DXBuffer** m_outBuffer = nullptr;

			DXBuffer* m_uploadBuffer = nullptr;
			DXHeap* m_uploadHeap = nullptr;

			DXBuffer* m_buffer = nullptr;
			DXHeap* m_heap = nullptr;

			int m_buffersToLoad = 2;

			jobs::Job* m_done = nullptr;
		};

		Context* ctx = new Context();
		ctx->m_geo = &geo;
		ctx->m_outBuffer = &buffer;
		ctx->m_done = done;

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
				*m_ctx.m_outBuffer = m_ctx.m_buffer;

				core::utils::RunSync(m_ctx.m_done);

				core::utils::DisposeBaseObject(*m_ctx.m_uploadBuffer);
				core::utils::DisposeBaseObject(*m_ctx.m_uploadHeap);

				delete& m_ctx;
			}
		};

		class BufferLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			BufferLoaded(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				--m_ctx.m_buffersToLoad;

				if (m_ctx.m_buffersToLoad > 0)
				{
					return;
				}

				m_ctx.m_uploadBuffer->CopyBuffer(*m_ctx.m_buffer, new Clear(m_ctx));
			}
		};

		class UploadData : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadData(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				void* data = m_ctx.m_uploadBuffer->Map();

				collada::Vertex* curDataPosition = static_cast<collada::Vertex*>(data);

				for (auto it = m_ctx.m_geo->m_vertices.begin(); it != m_ctx.m_geo->m_vertices.end(); ++it)
				{
					*curDataPosition = *it;
					++curDataPosition;
				}

				m_ctx.m_uploadBuffer->Unmap();

				core::utils::RunSync(new BufferLoaded(m_ctx));
			}
		};

		class UploadHeapResident : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadHeapResident(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);
				core::utils::RunAsync(new UploadData(m_ctx));
			}
		};

		class HeapResident : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			HeapResident(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
				core::utils::RunSync(new BufferLoaded(m_ctx));
			}
		};

		class CreateObjects : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateObjects(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				UINT64 stride = sizeof(collada::Vertex);
				UINT64 size = m_ctx.m_geo->m_vertices.size() * stride;

				m_ctx.m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_uploadBuffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_uploadBuffer->SetBufferStride(stride);

				m_ctx.m_uploadHeap = new DXHeap();
				m_ctx.m_uploadHeap->SetHeapSize(size);
				m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
				m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_uploadHeap->Create();

				m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_buffer->SetBufferStride(stride);

				m_ctx.m_heap = new DXHeap();
				m_ctx.m_heap->SetHeapSize(size);
				m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_heap->Create();

				m_ctx.m_uploadHeap->MakeResident(new UploadHeapResident(m_ctx));
				m_ctx.m_heap->MakeResident(new HeapResident(m_ctx));
			}
		};

		core::utils::RunSync(new CreateObjects(*ctx));
	}

	void LoadIndexBuffer(const collada::Geometry& geo, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		struct Context
		{
			const collada::Geometry* m_geo = nullptr;

			DXBuffer** m_outBuffer = nullptr;

			DXBuffer* m_uploadBuffer = nullptr;
			DXHeap* m_uploadHeap = nullptr;

			DXBuffer* m_buffer = nullptr;
			DXHeap* m_heap = nullptr;

			int m_buffersToLoad = 2;

			jobs::Job* m_done = nullptr;
		};

		Context* ctx = new Context();
		ctx->m_geo = &geo;
		ctx->m_outBuffer = &buffer;
		ctx->m_done = done;

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
				*m_ctx.m_outBuffer = m_ctx.m_buffer;

				core::utils::RunSync(m_ctx.m_done);

				core::utils::DisposeBaseObject(*m_ctx.m_uploadBuffer);
				core::utils::DisposeBaseObject(*m_ctx.m_uploadHeap);

				delete& m_ctx;
			}
		};

		class BufferLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			BufferLoaded(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				--m_ctx.m_buffersToLoad;

				if (m_ctx.m_buffersToLoad > 0)
				{
					return;
				}

				m_ctx.m_uploadBuffer->CopyBuffer(*m_ctx.m_buffer, new Clear(m_ctx));
			}
		};

		class UploadData : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadData(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				void* data = m_ctx.m_uploadBuffer->Map();

				int* curDataPosition = static_cast<int*>(data);

				for (auto it = m_ctx.m_geo->m_indices.begin(); it != m_ctx.m_geo->m_indices.end(); ++it)
				{
					*curDataPosition = *it;
					++curDataPosition;
				}

				m_ctx.m_uploadBuffer->Unmap();

				core::utils::RunSync(new BufferLoaded(m_ctx));
			}
		};

		class UploadHeapResident : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadHeapResident(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);
				core::utils::RunAsync(new UploadData(m_ctx));
			}
		};

		class HeapResident : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			HeapResident(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
				core::utils::RunSync(new BufferLoaded(m_ctx));
			}
		};

		class CreateObjects : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateObjects(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				UINT64 stride = sizeof(int);
				UINT64 size = m_ctx.m_geo->m_indices.size() * stride;

				m_ctx.m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_uploadBuffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_uploadBuffer->SetBufferStride(stride);

				m_ctx.m_uploadHeap = new DXHeap();
				m_ctx.m_uploadHeap->SetHeapSize(size);
				m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
				m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_uploadHeap->Create();

				m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_buffer->SetBufferStride(stride);

				m_ctx.m_heap = new DXHeap();
				m_ctx.m_heap->SetHeapSize(size);
				m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_heap->Create();

				m_ctx.m_uploadHeap->MakeResident(new UploadHeapResident(m_ctx));
				m_ctx.m_heap->MakeResident(new HeapResident(m_ctx));
			}
		};

		core::utils::RunSync(new CreateObjects(*ctx));
	}

	void LoadInstanceBuff(const collada::InstanceBuffer& instanceBuffer, rendering::DXMutableBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		struct Context
		{
			const collada::InstanceBuffer* m_instanceBuff = nullptr;

			DXMutableBuffer** m_outBuffer = nullptr;

			DXMutableBuffer* m_buffer = nullptr;

			jobs::Job* m_done = nullptr;
		};

		Context ctx;
		ctx.m_instanceBuff = &instanceBuffer;
		ctx.m_outBuffer = &buffer;
		ctx.m_done = done;

		class Finish : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			Finish(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				*m_ctx.m_outBuffer = m_ctx.m_buffer;
				core::utils::RunSync(m_ctx.m_done);
			}
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
				void* data = m_ctx.m_buffer->GetUploadBuffer()->Map();

				collada::GeometryInstanceData* curDataPosition = static_cast<collada::GeometryInstanceData*>(data);

				for (auto it = m_ctx.m_instanceBuff->m_data.begin(); it != m_ctx.m_instanceBuff->m_data.end(); ++it)
				{
					*curDataPosition = *it;
					++curDataPosition;
				}
				m_ctx.m_buffer->GetUploadBuffer()->Unmap();

				m_ctx.m_buffer->Copy(new Finish(m_ctx));
			}
		};


		class CreateObjects : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateObjects(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				UINT64 stride = sizeof(collada::GeometryInstanceData);
				UINT64 size = m_ctx.m_instanceBuff->m_data.size() * stride;

				m_ctx.m_buffer = new DXMutableBuffer(DXMutableBufferMeta::GetInstance(), size, stride, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_buffer->Load(new UploadData(m_ctx));
			}
		};

		core::utils::RunSync(new CreateObjects(ctx));
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

			m_context.m_dxScene->LoadBuffers(index, m_context.m_done);
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

void rendering::DXScene::LoadGeometryBuffers(int sceneIndex, const std::string& geometryName, SceneResources& sceneResources, jobs::Job* done)
{
	collada::ColladaScene* colladaScene = m_colladaScenes[sceneIndex];
	const collada::Scene& scene = colladaScene->GetScene();

	const collada::Geometry& geo = scene.m_geometries.find(geometryName)->second;
	const collada::InstanceBuffer& instanceBuffer = scene.m_instanceBuffers.find(geometryName)->second;

	struct Context
	{
		std::string m_geoName;
		SceneResources* m_sceneResources = nullptr;

		DXBuffer* m_vertexBuffer = nullptr;
		DXBuffer* m_indexBuffer = nullptr;
		DXMutableBuffer* m_instanceBuffer = nullptr;

		int m_jobsLeft = 3;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_geoName = geometryName;
	ctx->m_sceneResources = &sceneResources;
	ctx->m_done = done;

	class JobDone : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		JobDone(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_jobsLeft;
			if (m_ctx.m_jobsLeft > 0)
			{
				return;
			}

			GeometryResources tmp;
			tmp.m_vertexBuffer = m_ctx.m_vertexBuffer;
			tmp.m_indexBuffer = m_ctx.m_indexBuffer;
			tmp.m_instanceBuffer = m_ctx.m_instanceBuffer;

			m_ctx.m_sceneResources->m_geometryResources[m_ctx.m_geoName] = tmp;
			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	LoadVertexBuffer(geo, ctx->m_vertexBuffer, new JobDone(*ctx));
	LoadIndexBuffer(geo, ctx->m_indexBuffer, new JobDone(*ctx));
	LoadInstanceBuff(instanceBuffer, ctx->m_instanceBuffer, new JobDone(*ctx));
}

void rendering::DXScene::LoadBuffers(int sceneIndex, jobs::Job* done)
{
	collada::ColladaScene* colladaScene = m_colladaScenes[sceneIndex];
	const collada::Scene& scene = colladaScene->GetScene();

	SceneResources& sceneResources = m_sceneResources[sceneIndex];

	struct Context
	{
		int m_itemsToLoad = -1;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_itemsToLoad = scene.m_geometries.size();
	ctx->m_done = done;
	
	class ItemReady : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		ItemReady(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_itemsToLoad;
			if (m_ctx.m_itemsToLoad > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	for (auto it = scene.m_geometries.begin(); it != scene.m_geometries.end(); ++it)
	{
		LoadGeometryBuffers(sceneIndex, it->first, sceneResources, new ItemReady(*ctx));
	}
}

void rendering::DXScene::GetSceneBB(DirectX::XMVECTOR& minPoint, DirectX::XMVECTOR& maxPoint)
{
	using namespace DirectX;

	minPoint = maxPoint = XMVectorSet(0, 0, 0, 0);

	for (int i = 0; i < m_scenesLoaded; ++i)
	{
		collada::ColladaScene* cur = m_colladaScenes[i];

		const collada::Scene& curScene = cur->GetScene();
		for (auto it = curScene.m_objects.begin(); it != curScene.m_objects.end(); ++it)
		{
			const collada::Object& obj = it->second;
			const collada::Geometry& objGeo = curScene.m_geometries.find(obj.m_geometry)->second;

			for (auto vertexIt = objGeo.m_vertices.begin(); vertexIt != objGeo.m_vertices.end(); ++vertexIt)
			{
				XMVECTOR point = XMVectorSet(
					(*vertexIt).m_position[0],
					(*vertexIt).m_position[1],
					(*vertexIt).m_position[2],
					0);

				XMVECTOR rotation = XMVectorSet(
					obj.m_instanceData.m_rotation[1],
					obj.m_instanceData.m_rotation[2],
					obj.m_instanceData.m_rotation[3],
					obj.m_instanceData.m_rotation[0]);

				XMVECTOR rotationConj = XMQuaternionConjugate(rotation);

				XMVECTOR rotated = XMQuaternionMultiply(rotation, point);
				rotated = XMQuaternionMultiply(rotated, rotationConj);

				XMVECTOR scale = XMVectorSet(
					obj.m_instanceData.m_scale[0],
					obj.m_instanceData.m_scale[1],
					obj.m_instanceData.m_scale[2],
					0);

				XMVECTOR offset = XMVectorSet(
					obj.m_instanceData.m_position[0],
					obj.m_instanceData.m_position[1],
					obj.m_instanceData.m_position[2],
					0);

				rotated *= scale;
				rotated += offset;

				minPoint = XMVectorMin(minPoint, rotated);
				maxPoint = XMVectorMax(maxPoint, rotated);
			}
		}
	}
}