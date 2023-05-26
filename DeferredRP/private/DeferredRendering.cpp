#include "DeferredRendering.h"

#include "CoreUtils.h"

#include "resources/DXGBufferDuffuseTexMeta.h"
#include "resources/DXGBufferNormalTexMeta.h"
#include "resources/DXGBufferPositionTexMeta.h"
#include "resources/DXGBufferSpecularTexMeta.h"

#include "resources/DXGBufferAmbientLitTexMeta.h"
#include "resources/DXGBufferDiffuseLitTexMeta.h"
#include "resources/DXGBufferSpecularLitTexMeta.h"

#include "DXHeap.h"
#include "DXBufferMeta.h"

#include "DXDescriptorHeap.h"

#include "DXBuffer.h"
#include "DXTexture.h"

namespace
{
	rendering::DXTexture* m_duffuseTex = nullptr;
	rendering::DXTexture* m_specularTex = nullptr;
	rendering::DXTexture* m_surfaceNormalTex = nullptr;
	rendering::DXTexture* m_positionTex = nullptr;
	rendering::DXBuffer* m_renderTextureVertexBuffer = nullptr;

	rendering::DXTexture* m_ambientLitTex = nullptr;
	rendering::DXTexture* m_diffuseLitTex = nullptr;
	rendering::DXTexture* m_specularLitTex = nullptr;

	void LoadRenderTextureVertexBuffer(jobs::Job* done)
	{
		using namespace rendering;

		struct Vertex
		{
			float m_pos[2];
			float m_uv[2];
		};

		struct Context
		{
			DXBuffer* m_buffer = nullptr;
			DXBuffer* m_uploadBuffer = nullptr;

			DXHeap* m_heap = nullptr;
			DXHeap* m_uploadHeap = nullptr;

			UINT m_size = 6 * sizeof(Vertex);

			bool m_heapReady = false;
			bool m_uploadHeapReady = false;

			jobs::Job* m_done = nullptr;
		};

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
				m_renderTextureVertexBuffer = m_ctx.m_buffer;

				delete m_ctx.m_uploadBuffer;
				delete m_ctx.m_uploadHeap;

				core::utils::RunSync(m_ctx.m_done);

				delete &m_ctx;
			}
		};

		class CopyBuffers : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CopyBuffers(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadBuffer->CopyBuffer(*m_ctx.m_buffer, new Clear(m_ctx));
			}
		};

		class UploadReady : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			UploadReady(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_uploadHeapReady = true;

				if (!m_ctx.m_heapReady)
				{
					return;
				}

				core::utils::RunSync(new CopyBuffers(m_ctx));
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
				m_ctx.m_uploadBuffer->Place(m_ctx.m_uploadHeap, 0);
				void* dst = m_ctx.m_uploadBuffer->Map();
				Vertex* verts = static_cast<Vertex*>(dst);

				{
					verts[0].m_pos[0] = -1;
					verts[0].m_pos[1] = -1;
					verts[0].m_uv[0] = 0;
					verts[0].m_uv[1] = 1;
				}

				{
					verts[1].m_pos[0] = -1;
					verts[1].m_pos[1] = 1;
					verts[1].m_uv[0] = 0;
					verts[1].m_uv[1] = 0;
				}

				{
					verts[2].m_pos[0] = 1;
					verts[2].m_pos[1] = 1;
					verts[2].m_uv[0] = 1;
					verts[2].m_uv[1] = 0;
				}

				{
					verts[3].m_pos[0] = -1;
					verts[3].m_pos[1] = -1;
					verts[3].m_uv[0] = 0;
					verts[3].m_uv[1] = 1;
				}

				{
					verts[4].m_pos[0] = 1;
					verts[4].m_pos[1] = 1;
					verts[4].m_uv[0] = 1;
					verts[4].m_uv[1] = 0;
				}

				{
					verts[5].m_pos[0] = 1;
					verts[5].m_pos[1] = -1;
					verts[5].m_uv[0] = 1;
					verts[5].m_uv[1] = 1;
				}

				m_ctx.m_uploadBuffer->Unmap();
				core::utils::RunSync(new UploadReady(m_ctx));
			}
		};

		class DefaultReady : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			DefaultReady(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
				m_ctx.m_heapReady = true;

				if (!m_ctx.m_uploadHeapReady)
				{
					return;
				}

				core::utils::RunSync(new CopyBuffers(m_ctx));
			}
		};

		class CreateResources : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateResources(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_buffer->SetBufferSizeAndFlags(m_ctx.m_size, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_buffer->SetBufferStride(sizeof(Vertex));

				m_ctx.m_uploadBuffer = new DXBuffer(DXBufferMeta::GetInstance());
				m_ctx.m_uploadBuffer->SetBufferSizeAndFlags(m_ctx.m_size, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
				m_ctx.m_uploadBuffer->SetBufferStride(sizeof(Vertex));

				m_ctx.m_heap = new DXHeap();
				m_ctx.m_heap->SetHeapSize(m_ctx.m_size);
				m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_heap->Create();

				m_ctx.m_uploadHeap = new DXHeap();
				m_ctx.m_uploadHeap->SetHeapSize(m_ctx.m_size);
				m_ctx.m_uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
				m_ctx.m_uploadHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
				m_ctx.m_uploadHeap->Create();

				m_ctx.m_heap->MakeResident(new DefaultReady(m_ctx));
				m_ctx.m_uploadHeap->MakeResident(new UploadData(m_ctx));
			}
		};

		Context* ctx = new Context();
		ctx->m_done = done;

		core::utils::RunSync(new CreateResources(*ctx));
	}

	void LoadRenderTextures(jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			DXTexture* m_diffuse = nullptr;
			DXTexture* m_specular = nullptr;
			DXTexture* m_normal = nullptr;
			DXTexture* m_position = nullptr;

			DXHeap* m_diffuseHeap = nullptr;
			DXHeap* m_specularHeap = nullptr;
			DXHeap* m_normalHeap = nullptr;
			DXHeap* m_positionHeap = nullptr;

			int m_itemsToLoad = 4;

			jobs::Job* m_done = nullptr;
		};

		class HeapLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
			DXHeap& m_heap;
			DXTexture& m_tex;
		public:
			HeapLoaded(Context& ctx, DXHeap& heap, DXTexture& tex) :
				m_ctx(ctx),
				m_heap(heap),
				m_tex(tex)
			{
			}

			void Do() override
			{
				m_tex.Place(m_heap, 0);
				--m_ctx.m_itemsToLoad;

				if (m_ctx.m_itemsToLoad > 0)
				{
					return;
				}

				m_duffuseTex = m_ctx.m_diffuse;
				m_specularTex = m_ctx.m_specular;
				m_surfaceNormalTex = m_ctx.m_normal;
				m_positionTex = m_ctx.m_position;

				core::utils::RunSync(m_ctx.m_done);
				delete& m_ctx;
			}
		};

		class CreateTextures : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateTextures(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				Window* wnd = core::utils::GetWindow();
				m_ctx.m_diffuse = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferDuffuseTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
				m_ctx.m_specular = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferSpecularTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
				m_ctx.m_normal = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferNormalTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
				m_ctx.m_position = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferPositionTexMeta::GetInstance(), wnd->m_width, wnd->m_height);

				m_ctx.m_diffuseHeap = new DXHeap();
				m_ctx.m_diffuseHeap->SetHeapSize(m_ctx.m_diffuse->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_diffuseHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_diffuseHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_diffuseHeap->Create();

				m_ctx.m_specularHeap = new DXHeap();
				m_ctx.m_specularHeap->SetHeapSize(m_ctx.m_specular->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_specularHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_specularHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_specularHeap->Create();

				m_ctx.m_normalHeap = new DXHeap();
				m_ctx.m_normalHeap->SetHeapSize(m_ctx.m_normal->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_normalHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_normalHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_normalHeap->Create();

				m_ctx.m_positionHeap = new DXHeap();
				m_ctx.m_positionHeap->SetHeapSize(m_ctx.m_position->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_positionHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_positionHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_positionHeap->Create();

				m_ctx.m_diffuseHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_diffuseHeap, *m_ctx.m_diffuse));
				m_ctx.m_specularHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_specularHeap, *m_ctx.m_specular));
				m_ctx.m_normalHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_normalHeap, *m_ctx.m_normal));
				m_ctx.m_positionHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_positionHeap, *m_ctx.m_position));
			}
		};

		Context* ctx = new Context();
		ctx->m_done = done;

		core::utils::RunSync(new CreateTextures(*ctx));
	}


	void LoadRenderLitTextures(jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			DXTexture* m_ambientLit = nullptr;
			DXTexture* m_diffuseLit = nullptr;
			DXTexture* m_specularLit = nullptr;

			DXHeap* m_ambientLitHeap = nullptr;
			DXHeap* m_diffuseLitHeap = nullptr;
			DXHeap* m_specularLitHeap = nullptr;

			int m_itemsToLoad = 3;

			jobs::Job* m_done = nullptr;
		};

		class HeapLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
			DXHeap& m_heap;
			DXTexture& m_tex;
		public:
			HeapLoaded(Context& ctx, DXHeap& heap, DXTexture& tex) :
				m_ctx(ctx),
				m_heap(heap),
				m_tex(tex)
			{
			}

			void Do() override
			{
				m_tex.Place(m_heap, 0);
				--m_ctx.m_itemsToLoad;

				if (m_ctx.m_itemsToLoad > 0)
				{
					return;
				}

				m_ambientLitTex = m_ctx.m_ambientLit;
				m_diffuseLitTex = m_ctx.m_diffuseLit;
				m_specularLitTex = m_ctx.m_specularLit;

				core::utils::RunSync(m_ctx.m_done);
				delete& m_ctx;
			}
		};

		class CreateTextures : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			CreateTextures(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				Window* wnd = core::utils::GetWindow();
				m_ctx.m_ambientLit = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferAmbientLitTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
				m_ctx.m_diffuseLit = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferDiffuseLitTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
				m_ctx.m_specularLit = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferSpecularLitTexMeta::GetInstance(), wnd->m_width, wnd->m_height);

				m_ctx.m_ambientLitHeap = new DXHeap();
				m_ctx.m_ambientLitHeap->SetHeapSize(m_ctx.m_ambientLit->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_ambientLitHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_ambientLitHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_ambientLitHeap->Create();

				m_ctx.m_diffuseLitHeap = new DXHeap();
				m_ctx.m_diffuseLitHeap->SetHeapSize(m_ctx.m_diffuseLit->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_diffuseLitHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_diffuseLitHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_diffuseLitHeap->Create();

				m_ctx.m_specularLitHeap = new DXHeap();
				m_ctx.m_specularLitHeap->SetHeapSize(m_ctx.m_specularLit->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_specularLitHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_specularLitHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_specularLitHeap->Create();

				m_ctx.m_ambientLitHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_ambientLitHeap, *m_ctx.m_ambientLit));
				m_ctx.m_diffuseLitHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_diffuseLitHeap, *m_ctx.m_diffuseLit));
				m_ctx.m_specularLitHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_specularLitHeap, *m_ctx.m_specularLit));
			}
		};

		Context* ctx = new Context();
		ctx->m_done = done;

		core::utils::RunSync(new CreateTextures(*ctx));
	}
}

rendering::DXTexture* rendering::deferred::GetGBufferDiffuseTex()
{
	return m_duffuseTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferSpecularTex()
{
	return m_specularTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferNormalTex()
{
	return m_surfaceNormalTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferPositionTex()
{
	return m_positionTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferAmbientLitTex()
{
	return m_ambientLitTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferDiffuseLitTex()
{
	return m_diffuseLitTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferSpecularLitTex()
{
	return m_specularLitTex;
}

rendering::DXBuffer* rendering::deferred::GetRenderTextureVertexBuffer()
{
	return m_renderTextureVertexBuffer;
}

void rendering::deferred::LoadGBuffer(jobs::Job* done)
{
	struct Context
	{
		int m_itemsToLoad = 2;
		jobs::Job* m_done = nullptr;
	};

	class ItemLoaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		ItemLoaded(Context& ctx) :
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

	Context* ctx = new Context();
	ctx->m_done = done;

	LoadRenderTextures(new ItemLoaded(*ctx));
	LoadRenderTextureVertexBuffer(new ItemLoaded(*ctx));
}

void rendering::deferred::LoadGBufferLitTextures(jobs::Job* done)
{
	LoadRenderLitTextures(done);
}
