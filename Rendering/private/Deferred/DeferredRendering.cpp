#include "Deferred/DeferredRendering.h"

#include "RenderUtils.h"

#include "Deferred/DXGBufferDuffuseTexMeta.h"
#include "Deferred/DXGBufferNormalTexMeta.h"
#include "Deferred/DXGBufferPositionTexMeta.h"

#include "DXHeap.h"

namespace
{
	rendering::DXTexture* m_duffuseTex = nullptr;
	rendering::DXTexture* m_surfaceNormalTex = nullptr;
	rendering::DXTexture* m_positionTex = nullptr;
}

rendering::DXTexture* rendering::deferred::GetGBufferDiffuseTex()
{
	return m_duffuseTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferNormalTex()
{
	return m_surfaceNormalTex;
}

rendering::DXTexture* rendering::deferred::GetGBufferPositionTex()
{
	return m_positionTex;
}

void rendering::deferred::LoadGBuffer(jobs::Job* done)
{
	struct Context
	{
		DXTexture* m_diffuse = nullptr;
		DXTexture* m_normal = nullptr;
		DXTexture* m_position = nullptr;

		DXHeap* m_diffuseHeap = nullptr;
		DXHeap* m_normalHeap = nullptr;
		DXHeap* m_positionHeap = nullptr;

		int m_texturesToLoad = 3;

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
			m_tex.Place(m_heap.GetHeap(), 0);
			--m_ctx.m_texturesToLoad;

			if (m_ctx.m_texturesToLoad > 0)
			{
				return;
			}

			m_duffuseTex = m_ctx.m_diffuse;
			m_surfaceNormalTex = m_ctx.m_normal;
			m_positionTex = m_ctx.m_position;

			utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
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
			Window* wnd = utils::GetWindow();
			m_ctx.m_diffuse = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferDuffuseTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
			m_ctx.m_normal = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferNormalTexMeta::GetInstance(), wnd->m_width, wnd->m_height);
			m_ctx.m_position = DXTexture::CreateRenderTargetTexture(deferred::DXGBufferPositionTexMeta::GetInstance(), wnd->m_width, wnd->m_height);

			m_ctx.m_diffuseHeap = new DXHeap();
			m_ctx.m_diffuseHeap->SetHeapSize(m_ctx.m_diffuse->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_diffuseHeap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_diffuseHeap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
			m_ctx.m_diffuseHeap->Create();

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
			m_ctx.m_normalHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_normalHeap, *m_ctx.m_normal));
			m_ctx.m_positionHeap->MakeResident(new HeapLoaded(m_ctx, *m_ctx.m_positionHeap, *m_ctx.m_position));
		}
	};

	Context* ctx = new Context();
	ctx->m_done = done;

	utils::RunSync(new CreateTextures(*ctx));
}
