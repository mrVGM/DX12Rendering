#include "CascadedSM.h"

#include "CascadedSMMeta.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXTexture.h"

#include "CoreUtils.h"

#include "DXHeap.h"
#include "DXHeapMeta.h"

#include "DXDescriptorHeap.h"

#include "resources/DXShadowMapDSDescriptorHeapMeta.h"

#include "resources/DXShadowMapMeta.h"
#include "resources/DXShadowMapDSMeta.h"

namespace
{
	void LoadSMDSTex(rendering::DXTexture*& outTex, jobs::Job* done)
	{
		using namespace rendering;
		struct Context
		{
			DXTexture** m_outTex = nullptr;

			DXTexture* m_tex = nullptr;
			DXHeap* m_heap = nullptr;

			jobs::Job* m_done = nullptr;
		};

		Context ctx;
		ctx.m_outTex = &outTex;
		ctx.m_done = done;

		class PlaceTex : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			PlaceTex(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				m_ctx.m_tex->Place(*m_ctx.m_heap, 0);
				*m_ctx.m_outTex = m_ctx.m_tex;

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
				UINT size = CascadedSM::m_resolution;

				m_ctx.m_tex = DXTexture::CreateDepthStencilTexture(DXShadowMapDSMeta::GetInstance(), size, size);

				m_ctx.m_heap = new DXHeap();
				m_ctx.m_heap->SetHeapSize(m_ctx.m_tex->GetTextureAllocationInfo().SizeInBytes);
				m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
				m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
				m_ctx.m_heap->Create();

				m_ctx.m_heap->MakeResident(new PlaceTex(m_ctx));
			}
		};

		core::utils::RunSync(new CreateTex(ctx));
	}
}

const UINT rendering::CascadedSM::m_resolution = 2048;

void rendering::CascadedSM::LoadMatrixBuffer(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		DXBuffer* m_buffer = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

	Context ctx;
	ctx.m_cascadedSM = this;
	ctx.m_done = done;

	class PlaceBuffer : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		PlaceBuffer(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_buffer->Place(m_ctx.m_heap, 0);
			m_ctx.m_cascadedSM->m_matrixBuffer = m_ctx.m_buffer;

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
			UINT size = 256;
			UINT stride = 256;

			m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
			m_ctx.m_buffer->SetBufferSizeAndFlags(size, D3D12_RESOURCE_FLAG_NONE);
			m_ctx.m_buffer->SetBufferStride(stride);

			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(size);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_heap->Create();

			m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx));
		}
	};

	core::utils::RunSync(new CreateBuffer(ctx));
}

void rendering::CascadedSM::LoadSMTexture(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		DXTexture* m_tex = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

	Context ctx;
	ctx.m_cascadedSM = this;
	ctx.m_done = done;

	class PlaceTex : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		PlaceTex(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_tex->Place(*m_ctx.m_heap, 0);
			m_ctx.m_cascadedSM->m_smTex = m_ctx.m_tex;

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
			UINT size = CascadedSM::m_resolution;

			m_ctx.m_tex = DXTexture::CreateRenderTargetTexture(DXShadowMapMeta::GetInstance(), size, size);

			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_tex->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
			m_ctx.m_heap->Create();

			m_ctx.m_heap->MakeResident(new PlaceTex(m_ctx));
		}
	};

	core::utils::RunSync(new CreateTex(ctx));
}

void rendering::CascadedSM::LoadDepthTextures(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		int m_itemsToWaitFor = 4;

		DXTexture* m_textures[4];

		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_cascadedSM = this;
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
			--m_ctx.m_itemsToWaitFor;

			if (m_ctx.m_itemsToWaitFor > 0)
			{
				return;
			}
			
			for (int i = 0; i < 4; ++i)
			{
				m_ctx.m_cascadedSM->m_depthTextures.push_back(m_ctx.m_textures[i]);
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	for (int i = 0; i < 4; ++i)
	{
		LoadSMDSTex(ctx->m_textures[i], new ItemReady(*ctx));
	}
}

void rendering::CascadedSM::CreateDescriptorHeaps()
{
	m_dsDescriptorHeap = DXDescriptorHeap::CreateDSVDescriptorHeap(
		DXShadowMapDSDescriptorHeapMeta::GetInstance(),
		m_depthTextures);
}

rendering::CascadedSM::CascadedSM() :
	BaseObject(CascadedSMMeta::GetInstance())
{
}

rendering::CascadedSM::~CascadedSM()
{
}


void rendering::CascadedSM::LoadResources(jobs::Job* done)
{
	struct Context
	{
		int m_itemsToWaitFor = 3;

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
			--m_ctx.m_itemsToWaitFor;

			if (m_ctx.m_itemsToWaitFor > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);
			delete& m_ctx;
		}
	};

	LoadMatrixBuffer(new ItemReady(*ctx));
	LoadDepthTextures(new ItemReady(*ctx));
	LoadSMTexture(new ItemReady(*ctx));
}