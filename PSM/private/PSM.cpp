#include "PSM.h"

#include "PSMMeta.h"

#include "resources/DXShadowMapMeta.h"
#include "resources/DXShadowMapDSMeta.h"
#include "resources/DXShadowMaskMeta.h"
#include "resources/DXSMSettingsBufferMeta.h"
#include "resources/DXSMDescriptorHeapMeta.h"

#include "DXHeap.h"
#include "DXTexture.h"

#include "DXMutableBuffer.h"
#include "DXDescriptorHeap.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	rendering::Window* m_wnd = nullptr;
	rendering::DXDevice* m_device = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;

	void CacheObjects()
	{
		if (!m_wnd)
		{
			m_wnd = rendering::core::utils::GetWindow();
		}

		if (!m_device)
		{
			m_device = rendering::core::utils::GetDevice();
		}

		if (!m_commandQueue)
		{
			m_commandQueue = rendering::core::utils::GetCommandQueue();
		}
	}
}


const UINT64 rendering::psm::PSM::m_resolution = 1024;

rendering::psm::PSM::PSM() :
	shadow_mapping::ShadowMap(rendering::psm::PSMMeta::GetInstance())
{
	CacheObjects();

	{
		THROW_ERROR(
			m_device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
			"Can't create Command Allocator!")

		THROW_ERROR(
			m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_preSMRenderList)),
			"Can't reset Command List!")

		THROW_ERROR(
			m_preSMRenderList->Close(),
			"Can't close Command List!")

		THROW_ERROR(
			m_device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_postSMRenderList)),
			"Can't reset Command List!")

		THROW_ERROR(
			m_postSMRenderList->Close(),
			"Can't close Command List!")
	}
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
			m_ctx.m_heap->Create();

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
			m_ctx.m_heap->Create();

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
			m_ctx.m_heap->Create();

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
		PSM* m_psm = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, done };

	class Ready : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		Ready(Context ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_psm->CreateDescriptorHeap();
			core::utils::RunSync(m_ctx.m_done);
		}
	};

	LoadResourcesInternal(new Ready(ctx));
}

void rendering::psm::PSM::LoadResourcesInternal(jobs::Job* done)
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

void rendering::psm::PSM::UpdateSMSettings()
{

}

void rendering::psm::PSM::CreateDescriptorHeap()
{
	std::list<DXTexture*> textures;
	textures.push_back(m_sm);
	textures.push_back(m_shadowMask);

	m_smDescriptorHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(DXSMDescriptorHeapMeta::GetInstance(), textures);
}



void rendering::psm::PSM::PreparePreSMRenderList()
{
	if (m_preSMRenderListPrepared)
	{
		return;
	}

	THROW_ERROR(
		m_preSMRenderList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_sm->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_shadowMask->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		};
		m_preSMRenderList->ResourceBarrier(_countof(barrier), barrier);
	}

	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		m_preSMRenderList->ClearRenderTargetView(m_smDescriptorHeap->GetDescriptorHandle(0), clearColor, 0, nullptr);
		m_preSMRenderList->ClearRenderTargetView(m_smDescriptorHeap->GetDescriptorHandle(1), clearColor, 0, nullptr);
	}

	THROW_ERROR(
		m_preSMRenderList->Close(),
		"Can't close Command List!")

	m_preSMRenderListPrepared = true;
}

void rendering::psm::PSM::PreparePostSMRenderList()
{
	if (m_postSMRenderListPrepared)
	{
		return;
	}

	THROW_ERROR(
		m_postSMRenderList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_sm->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_shadowMask->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
		};
		m_postSMRenderList->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		m_postSMRenderList->Close(),
		"Can't close Command List!")

	m_postSMRenderListPrepared = true;
}


void rendering::psm::PSM::RenderShadowMask()
{
	PreparePreSMRenderList();
	PreparePostSMRenderList();

	{
		ID3D12CommandList* ppCommandLists[] = { m_preSMRenderList.Get() };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}


	{
		ID3D12CommandList* ppCommandLists[] = { m_postSMRenderList.Get() };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
}

rendering::DXTexture* rendering::psm::PSM::GetShadowMask()
{
	return m_shadowMask;
}

#undef THROW_ERROR