#include "Lights/LightsManager.h"

#include "Lights/LightsManagerMeta.h"

#include "Lights/DXShadowMapMeta.h"
#include "Lights/DXShadowMapDSMeta.h"

#include "RenderUtils.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXHeap.h"
#include "DXTexture.h"
#include "DXHeap.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"
#include "DXShadowMapDSDescriptorHeapMeta.h"

namespace
{
	struct LightsBuffer
	{
		int m_numLights;
		float m_placeholder[3];

		rendering::Light m_lights[15];
	};
}

rendering::LightsManager::LightsManager() :
	BaseObject(LightsManagerMeta::GetInstance())
{

	Light l;
	l.m_position[0] = 3;
	l.m_position[1] = 3;
	l.m_position[2] = -3;
	l.m_range = 30;

	AddLight(l);
}

rendering::LightsManager::~LightsManager()
{
}

void rendering::LightsManager::AddLight(const Light& light)
{
	m_lights.push_back(light);
}

void rendering::LightsManager::LoadLightsBuffer(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_lightsManager = nullptr;

		DXBuffer* m_buffer = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

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

			void* data = m_ctx.m_buffer->Map();
			LightsBuffer* lightsBufferData = static_cast<LightsBuffer*>(data);

			lightsBufferData->m_numLights = m_ctx.m_lightsManager->m_lights.size();
			if (lightsBufferData->m_numLights > 15)
			{
				lightsBufferData->m_numLights = 15;
			}

			int index = 0;
			for (auto it = m_ctx.m_lightsManager->m_lights.begin(); it != m_ctx.m_lightsManager->m_lights.end(); ++it)
			{
				if (index >= lightsBufferData->m_numLights)
				{
					break;
				}

				lightsBufferData->m_lights[index++] = *it;
			}

			m_ctx.m_buffer->Unmap();

			m_ctx.m_lightsManager->m_lightsBuffer = m_ctx.m_buffer;
			utils::RunSync(m_ctx.m_done);
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
			m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
			m_ctx.m_buffer->SetBufferSizeAndFlags(256, D3D12_RESOURCE_FLAG_NONE);
			m_ctx.m_buffer->SetBufferStride(256);

			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(256);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_heap->Create();

			m_ctx.m_heap->MakeResident(new PlaceBuffer(m_ctx));
		}
	};

	Context ctx;
	ctx.m_lightsManager = this;
	ctx.m_done = done;

	utils::RunSync(new CreateObjects(ctx));
}

void rendering::LightsManager::LoadShadowMapDSTex(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_manager = nullptr;
		DXTexture* m_texture = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

	class PlaceTexture : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		PlaceTexture(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_texture->Place(*m_ctx.m_heap, 0);
			m_ctx.m_manager->m_shadowMapDepthStencil = m_ctx.m_texture;

			m_ctx.m_manager->m_shadowMapDSDescriptorHeap = 
				DXDescriptorHeap::CreateDSVDescriptorHeap(DXShadowMapDSDescriptorHeapMeta::GetInstance(), *m_ctx.m_manager->m_shadowMapDepthStencil);

			utils::RunSync(m_ctx.m_done);
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

		void Do()
		{
			m_ctx.m_texture = DXTexture::CreateDepthStencilTexture(DXShadowMapDSMeta::GetInstance(), 600, 600);

			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_texture->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			m_ctx.m_heap->Create();

			m_ctx.m_heap->MakeResident(new PlaceTexture(m_ctx));
		}
	};

	Context ctx
	{
		this,
		nullptr,
		nullptr,
		done
	};

	utils::RunSync(new CreateTex(ctx));
}

void rendering::LightsManager::CreateDescriptorHeaps()
{
	std::list<DXTexture*> textures;
	textures.push_back(m_shadowMap);
	m_shadowMapRTV = DXDescriptorHeap::CreateRTVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
	m_shadowMapSRV = DXDescriptorHeap::CreateSRVDescriptorHeap(DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::LightsManager::LoadShadowMapTex(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_manager = nullptr;
		DXTexture* m_texture = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

	class PlaceTexture : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		PlaceTexture(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_texture->Place(*m_ctx.m_heap, 0);
			m_ctx.m_manager->m_shadowMap = m_ctx.m_texture;

			utils::RunSync(m_ctx.m_done);
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

		void Do()
		{
			m_ctx.m_texture = DXTexture::CreateRenderTargetTexture(DXShadowMapMeta::GetInstance(), 600, 600);
			
			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapSize(m_ctx.m_texture->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			m_ctx.m_heap->Create();

			m_ctx.m_heap->MakeResident(new PlaceTexture(m_ctx));
		}
	};

	Context ctx
	{
		this,
		nullptr,
		nullptr,
		done
	};

	utils::RunSync(new CreateTex(ctx));
}


void rendering::LightsManager::LoadShadowMap(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_lightsManager = nullptr;
		int m_itemsToLoad = 2;
		jobs::Job* m_done = nullptr;
	};

	class ItemDone : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		ItemDone(Context& ctx) :
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

			m_ctx.m_lightsManager->CreateDescriptorHeaps();

			utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};


	Context* ctx = new Context();
	ctx->m_lightsManager = this;
	ctx->m_done = done;

	LoadShadowMapTex(new ItemDone(*ctx));
	LoadShadowMapDSTex(new ItemDone(*ctx));
}


rendering::DXBuffer* rendering::LightsManager::GetLightsBuffer()
{
	return m_lightsBuffer;
}

rendering::DXTexture* rendering::LightsManager::GetShadowMap()
{
	return m_shadowMap;
}

rendering::DXTexture* rendering::LightsManager::GetShadowMapDepthStencil()
{
	return m_shadowMapDepthStencil;
}

rendering::DXDescriptorHeap* rendering::LightsManager::GetShadowMapDSDescriptorHeap()
{
	return m_shadowMapDSDescriptorHeap;
}

rendering::DXDescriptorHeap* rendering::LightsManager::GetSMRTVHeap()
{
	return m_shadowMapRTV;
}

rendering::DXDescriptorHeap* rendering::LightsManager::GetSMSRVHeap()
{
	return m_shadowMapSRV;
}
