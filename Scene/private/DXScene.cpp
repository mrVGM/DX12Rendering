#include "DXScene.h"

#include "DXSceneMeta.h"

#include "CoreUtils.h"

#include "Job.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"
#include "DXMutableBuffer.h"
#include "DXMutableBufferMeta.h"
#include "DXHeap.h"

#include "Notifications.h"
#include "SceneLoadedNotificationMeta.h"

#include "SceneMaterialsSettings.h"

#include "ColladaEntities.h"
#include "utils.h"

#include <list>

namespace
{
	collada::SceneSettings* m_sceneSettings = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_sceneSettings)
		{
			m_sceneSettings = GetSceneSettings();
		}
	}

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

	void LoadSkeletalMeshVertexBuffer(const collada::Skeleton* skeleton, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		if (!skeleton)
		{
			core::utils::RunSync(done);
			return;
		}

		struct Context
		{
			const collada::Skeleton* m_skel = nullptr;

			DXBuffer** m_outBuffer = nullptr;

			DXBuffer* m_uploadBuffer = nullptr;
			DXHeap* m_uploadHeap = nullptr;

			DXBuffer* m_buffer = nullptr;
			DXHeap* m_heap = nullptr;

			int m_buffersToLoad = 2;

			jobs::Job* m_done = nullptr;
		};

		Context* ctx = new Context();
		ctx->m_skel = skeleton;
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


				std::map<std::string, int> jointIDs;
				{
					int index = 0;
					for (auto it = m_ctx.m_skel->m_joints.begin(); it != m_ctx.m_skel->m_joints.end(); ++it)
					{
						jointIDs[*it] = index++;
					}
				}

				collada::SkeletalMeshVertexWeights* curDataPosition = static_cast<collada::SkeletalMeshVertexWeights*>(data);
				for (auto it = m_ctx.m_skel->m_weights.begin(); it != m_ctx.m_skel->m_weights.end(); ++it)
				{
					collada::SkeletalMeshVertexWeights& curSkeletalMeshVertex = *curDataPosition++;

					int index = 0;
					for (auto weightIt = (*it).begin(); weightIt != (*it).end(); ++weightIt)
					{
						if (index >= 4)
						{
							break;
						}

						curSkeletalMeshVertex.m_joints[index] = jointIDs[(*weightIt).m_joint];
						curSkeletalMeshVertex.m_weights[index] = (*weightIt).m_weight;

						++index;
					}
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
				UINT64 stride = sizeof(collada::SkeletalMeshVertexWeights);
				UINT64 size = m_ctx.m_skel->m_weights.size() * stride;

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

	void LoadSkeletonJointsBuffer(const collada::SkeletonBuffer* skeleton, rendering::DXBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		if (!skeleton)
		{
			core::utils::RunSync(done);
			return;
		}

		struct Context
		{
			const collada::SkeletonBuffer* m_skel = nullptr;

			DXBuffer** m_outBuffer = nullptr;

			DXBuffer* m_uploadBuffer = nullptr;
			DXHeap* m_uploadHeap = nullptr;

			DXBuffer* m_buffer = nullptr;
			DXHeap* m_heap = nullptr;

			int m_buffersToLoad = 2;

			jobs::Job* m_done = nullptr;
		};

		Context* ctx = new Context();
		ctx->m_skel = skeleton;
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

				collada::Matrix* curDataPosition = static_cast<collada::Matrix*>(data);

				*curDataPosition++ = m_ctx.m_skel->m_bindPoseMatrix;
				for (auto it = m_ctx.m_skel->m_invBindPoseMatrices.begin(); it != m_ctx.m_skel->m_invBindPoseMatrices.end(); ++it)
				{
					*curDataPosition++ = *it;
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
				UINT64 stride = sizeof(collada::Matrix);
				UINT64 size = (m_ctx.m_skel->m_invBindPoseMatrices.size() + 1) * stride;

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

		class DataUploaded : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			DataUploaded(const Context& ctx) :
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
				m_ctx.m_buffer->Upload(new DataUploaded(m_ctx));
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

				m_ctx.m_buffer = new DXMutableBuffer(DXMutableBufferMeta::GetInstance(), size, stride);
				m_ctx.m_buffer->Load(new UploadData(m_ctx));
			}
		};

		core::utils::RunSync(new CreateObjects(ctx));
	}

	void LoadSkeletonPoseBuff(const collada::SkeletonPoseBuffer* poseBuffer, rendering::DXMutableBuffer*& buffer, jobs::Job* done)
	{
		using namespace rendering;

		if (!poseBuffer)
		{
			core::utils::RunSync(done);
			return;
		}

		struct Context
		{
			const collada::SkeletonPoseBuffer* m_poseBuff = nullptr;

			DXMutableBuffer** m_outBuffer = nullptr;

			DXMutableBuffer* m_buffer = nullptr;

			jobs::Job* m_done = nullptr;
		};

		Context ctx;
		ctx.m_poseBuff = poseBuffer;
		ctx.m_outBuffer = &buffer;
		ctx.m_done = done;

		class DataUploaded : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			DataUploaded(const Context& ctx) :
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

				collada::Matrix* curDataPosition = static_cast<collada::Matrix*>(data);

				for (auto it = m_ctx.m_poseBuff->m_jointTransforms.begin(); it != m_ctx.m_poseBuff->m_jointTransforms.end(); ++it)
				{
					*curDataPosition = *it;
					++curDataPosition;
				}

				m_ctx.m_buffer->GetUploadBuffer()->Unmap();
				m_ctx.m_buffer->Upload(new DataUploaded(m_ctx));
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
				UINT64 stride = sizeof(collada::Matrix);
				UINT64 size = m_ctx.m_poseBuff->m_jointTransforms.size() * stride;

				m_ctx.m_buffer = new DXMutableBuffer(DXMutableBufferMeta::GetInstance(), size, stride);
				m_ctx.m_buffer->Load(new UploadData(m_ctx));
			}
		};

		core::utils::RunSync(new CreateObjects(ctx));
	}
}


rendering::DXScene::DXScene() :
	BaseObject(DXSceneMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXScene::~DXScene()
{
}

void rendering::DXScene::LoadColladaScene(const std::string& sceneId, jobs::Job* done)
{
	struct JobContext
	{
		DXScene* m_dxScene = nullptr;
		std::string m_sceneId;
		collada::ColladaScene* m_scene = nullptr;
		SceneMaterialsSettings* m_materialSettingsReader = nullptr;

		jobs::Job* m_done = nullptr;
	};

	class BuffersLoaded : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		BuffersLoaded(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			++m_context.m_dxScene->m_scenesLoaded;

			notifications::Notify(SceneLoadedNotificationMeta::GetInstance());

			core::utils::RunSync(m_context.m_done);
		}
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

			m_context.m_dxScene->LoadBuffers(index, new BuffersLoaded(m_context));
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
			collada::SceneSettings::Settings& sceneSettings = m_sceneSettings->GetSettings();
			collada::SceneSettings::SceneInfo& sceneInfo = sceneSettings.m_scenes[m_context.m_sceneId];

			data::MemoryFile mf;
			mf.RestoreFromFile(data::GetLibrary().GetRootDir() + sceneInfo.m_binFile);

			data::MemoryFileReader reader(mf);
			m_context.m_scene->GetScene().Deserialize(reader);
			m_context.m_scene->GetScene().ConstructInstanceBuffers();
			m_context.m_scene->GetScene().ConstructSkeletonBuffers();

			m_context.m_materialSettingsReader->LoadSceneMaterialsSettings(m_context.m_scene->GetScene().m_materials);

			core::utils::DisposeBaseObject(*m_context.m_materialSettingsReader);
			core::utils::RunSync(new PostLoadColladaSceneJob(m_context));
		}
	};

	class CreateMaterialsReader : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		CreateMaterialsReader(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			collada::SceneSettings::Settings& sceneSettings = m_sceneSettings->GetSettings();
			collada::SceneSettings::SceneInfo& sceneInfo = sceneSettings.m_scenes[m_context.m_sceneId];
			m_context.m_materialSettingsReader = new SceneMaterialsSettings(sceneInfo.m_materialsFile);

			core::utils::RunAsync(new LoadColladaSceneJob(m_context));
		}
	};

	JobContext ctx{ this, sceneId, new collada::ColladaScene(), nullptr, done };
	core::utils::RunSync(new CreateMaterialsReader(ctx));
}

void rendering::DXScene::LoadGeometryBuffers(int sceneIndex, const std::string& geometryName, SceneResources& sceneResources, jobs::Job* done)
{
	collada::ColladaScene* colladaScene = m_colladaScenes[sceneIndex];
	const collada::Scene& scene = colladaScene->GetScene();

	const collada::Geometry& geo = scene.m_geometries.find(geometryName)->second;
	const collada::InstanceBuffer& instanceBuffer = scene.m_instanceBuffers.find(geometryName)->second;

	const collada::Skeleton* skeleton = nullptr;
	{
		auto it = scene.m_skeletons.find(geometryName);
		if (it != scene.m_skeletons.end())
		{
			skeleton = &it->second;
		}
	}
	const collada::SkeletonBuffer* skeletonBuffer = nullptr;
	{
		auto it = scene.m_skeletonBuffers.find(geometryName);
		if (it != scene.m_skeletonBuffers.end())
		{
			skeletonBuffer = &it->second;
		}
	}

	struct Context
	{
		std::string m_geoName;
		SceneResources* m_sceneResources = nullptr;

		DXBuffer* m_vertexBuffer = nullptr;
		DXBuffer* m_indexBuffer = nullptr;
		DXMutableBuffer* m_instanceBuffer = nullptr;

		DXBuffer* m_skeletalMeshVertexBuffer = nullptr;
		DXBuffer* m_skeletonBuffer = nullptr;

		int m_jobsLeft = 5;
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
			tmp.m_skeletalMeshVertexBuffer = m_ctx.m_skeletalMeshVertexBuffer;
			tmp.m_skeletonBuffer = m_ctx.m_skeletonBuffer;

			m_ctx.m_sceneResources->m_geometryResources[m_ctx.m_geoName] = tmp;
			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	LoadVertexBuffer(geo, ctx->m_vertexBuffer, new JobDone(*ctx));
	LoadIndexBuffer(geo, ctx->m_indexBuffer, new JobDone(*ctx));
	LoadInstanceBuff(instanceBuffer, ctx->m_instanceBuffer, new JobDone(*ctx));

	LoadSkeletalMeshVertexBuffer(skeleton, ctx->m_skeletalMeshVertexBuffer, new JobDone(*ctx));
	LoadSkeletonJointsBuffer(skeletonBuffer, ctx->m_skeletonBuffer, new JobDone(*ctx));
}

void rendering::DXScene::LoadObjectBuffers(int sceneIndex, const std::string& objectName, SceneResources& sceneResources, jobs::Job* done)
{
	collada::ColladaScene* colladaScene = m_colladaScenes[sceneIndex];
	const collada::Scene& scene = colladaScene->GetScene();
	
	const collada::SkeletonPoseBuffer* poseBuffer = nullptr;
	{
		auto it = scene.m_skeletonPoseBuffers.find(objectName);
		if (it != scene.m_skeletonPoseBuffers.end())
		{
			poseBuffer = &(it->second);
		}
	}

	struct Context
	{
		std::string m_objName;
		SceneResources* m_sceneResources = nullptr;

		DXMutableBuffer* m_poseBuffer = nullptr;

		int m_jobsLeft = 1;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_objName = objectName;
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

			ObjectResources tmp;
			tmp.m_skeletonPoseBuffer = m_ctx.m_poseBuffer;

			m_ctx.m_sceneResources->m_objectResources[m_ctx.m_objName] = tmp;
			core::utils::RunSync(m_ctx.m_done);
			delete& m_ctx;
		}
	};

	LoadSkeletonPoseBuff(poseBuffer, ctx->m_poseBuffer, new JobDone(*ctx));
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
	ctx->m_itemsToLoad = scene.m_geometries.size() + scene.m_objects.size();
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

	for (auto it = scene.m_objects.begin(); it != scene.m_objects.end(); ++it)
	{
		LoadObjectBuffers(sceneIndex, it->first, sceneResources, new ItemReady(*ctx));
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

int rendering::DXScene::GetScenesCount()
{
	return m_scenesLoaded;
}