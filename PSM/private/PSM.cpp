#include "PSM.h"

#include "PSMMeta.h"

#include "resources/DXShadowMapMeta.h"
#include "resources/DXShadowMapDSMeta.h"
#include "resources/DXShadowMaskMeta.h"
#include "resources/DXSMSettingsBufferMeta.h"

#include "DXHeap.h"
#include "DXTexture.h"

#include "DXMutableBuffer.h"

#include "CoreUtils.h"

namespace
{
	rendering::Window* m_wnd = nullptr;

	void CacheObjects()
	{
		if (!m_wnd)
		{
			m_wnd = rendering::core::utils::GetWindow();
		}
	}
}


const UINT64 rendering::psm::PSM::m_resolution = 1024;

rendering::psm::PSM::PSM() :
	shadow_mapping::ShadowMap(rendering::psm::PSMMeta::GetInstance())
{
	CacheObjects();
}

rendering::psm::PSM::~PSM()
{
}

void rendering::psm::PSM::LoadSMTex(jobs::Job* done)
{
	struct Context
	{
		rendering::psm::PSM* m_psm = nullptr;
		DXTexture* m_tex = nullptr;
		DXHeap* m_heap = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, nullptr, nullptr, done };

	class HeapReady : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		HeapReady(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex->Place(*m_ctx.m_heap, 0);
			m_ctx.m_psm->m_sm = m_ctx.m_tex;
			core::utils::RunSync(m_ctx.m_done);
		}
	};

	class CreateTex : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CreateTex(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex = DXTexture::CreateRenderTargetTexture(DXShadowMapMeta::GetInstance(), PSM::m_resolution, PSM::m_resolution);
			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_tex->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			m_ctx.m_heap->MakeResident(new HeapReady(m_ctx));
		}
	};

	core::utils::RunSync(new CreateTex(ctx));
}


void rendering::psm::PSM::LoadSMDSTex(jobs::Job* done)
{
	struct Context
	{
		rendering::psm::PSM* m_psm = nullptr;
		DXTexture* m_tex = nullptr;
		DXHeap* m_heap = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, nullptr, nullptr, done };

	class HeapReady : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		HeapReady(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex->Place(*m_ctx.m_heap, 0);
			m_ctx.m_psm->m_smDS = m_ctx.m_tex;
			core::utils::RunSync(m_ctx.m_done);
		}
	};

	class CreateTex : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CreateTex(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex = DXTexture::CreateDepthStencilTexture(DXShadowMapDSMeta::GetInstance(), PSM::m_resolution, PSM::m_resolution);
			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_tex->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			m_ctx.m_heap->MakeResident(new HeapReady(m_ctx));
		}
	};

	core::utils::RunSync(new CreateTex(ctx));
}


void rendering::psm::PSM::LoadShadowMaskTex(jobs::Job* done)
{
	struct Context
	{
		rendering::psm::PSM* m_psm = nullptr;
		DXTexture* m_tex = nullptr;
		DXHeap* m_heap = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, nullptr, nullptr, done };

	class HeapReady : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		HeapReady(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex->Place(*m_ctx.m_heap, 0);
			m_ctx.m_psm->m_shadowMask = m_ctx.m_tex;
			core::utils::RunSync(m_ctx.m_done);
		}
	};

	class CreateTex : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CreateTex(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex = DXTexture::CreateRenderTargetTexture(DXShadowMaskMeta::GetInstance(), m_wnd->m_width, m_wnd->m_height);
			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_tex->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			m_ctx.m_heap->MakeResident(new HeapReady(m_ctx));
		}
	};

	core::utils::RunSync(new CreateTex(ctx));
}


void rendering::psm::PSM::LoadSettingsBuffer(jobs::Job* done)
{
	struct Context
	{
		rendering::psm::PSM* m_psm = nullptr;
		DXMutableBuffer* m_buffer = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, nullptr, done };

	class BufferReady : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		BufferReady(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_psm->m_settingsBuffer = m_ctx.m_buffer;
			core::utils::RunSync(m_ctx.m_done);
		}
	};

	class CreateBuffer : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CreateBuffer(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_buffer = new DXMutableBuffer(DXSMSettingsBufferMeta::GetInstance(), 256, 256);
			m_ctx.m_buffer->Load(new BufferReady(m_ctx));
		}
	};

	core::utils::RunSync(new CreateBuffer(ctx));
}


void rendering::psm::PSM::LoadResources(jobs::Job* done)
{
	struct Context
	{
		int m_itemsLoading = 4;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
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
			--m_ctx.m_itemsLoading;
			if (m_ctx.m_itemsLoading > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	LoadSMTex(new ItemReady(*ctx));
	LoadSMDSTex(new ItemReady(*ctx));
	LoadShadowMaskTex(new ItemReady(*ctx));
	LoadSettingsBuffer(new ItemReady(*ctx));
}

rendering::DXMutableBuffer* rendering::psm::PSM::GetSettingsBuffer()
{
	return m_settingsBuffer;
}