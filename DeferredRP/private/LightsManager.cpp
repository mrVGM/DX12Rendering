#include "LightsManager.h"

#include "LightsManagerMeta.h"

#include "DXShadowMapMeta.h"
#include "DXShadowMapDSMeta.h"

#include "CoreUtils.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXHeap.h"
#include "DXTexture.h"
#include "DXHeap.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"
#include "DXShadowMapDSDescriptorHeapMeta.h"

#include "DXLightsBufferMeta.h"
#include "DXSMSettingsBufferMeta.h"

#include "ICamera.h"
#include "ICameraMeta.h"

#include "BaseObjectContainer.h"

#include "DXShadowMapUpdater.h"
#include "DXShadowMapRDU.h"

#include <DirectXMath.h>

namespace
{
	rendering::DXDevice* m_device = nullptr;
	rendering::ICamera* m_camera = nullptr;
	

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_device)
		{
			m_device = core::utils::GetDevice();
		}

		if (!m_camera)
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();
			BaseObject* obj = container.GetObjectOfClass(ICameraMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Camera!";
			}

			m_camera = static_cast<ICamera*>(obj);
		}
	}

	struct LightsBuffer
	{
		int m_numLights;
		float m_placeholder[3];

		rendering::Light m_lights[15];
	};


	struct ShadowMapSettings
	{
		float m_matrix[16];
		float m_position[4];
	};

	

	DirectX::XMMATRIX GetTransformMatrixFromVectors(
		const DirectX::XMVECTOR& origin,
		const DirectX::XMVECTOR& target,
		const DirectX::XMVECTOR& right,
		const DirectX::XMVECTOR& up,
		const DirectX::XMVECTOR& frustrumSettings)
	{
		using namespace DirectX;

		float eps = 0.0000001f;

		float farPlane = XMVectorGetX(frustrumSettings);
		float nearPlane = XMVectorGetY(frustrumSettings);
		float fov = XMVectorGetZ(frustrumSettings);
		float aspect = XMVectorGetW(frustrumSettings);

		XMVECTOR fwd = target - origin;

		fwd = XMVector3Normalize(fwd);

		float fovRad = DirectX::XMConvertToRadians(fov);

		float h = tan(fovRad / 2);
		float w = aspect * h;

		DirectX::XMMATRIX translate(
			DirectX::XMVECTOR{ 1, 0, 0, -XMVectorGetX(origin) },
			DirectX::XMVECTOR{ 0, 1, 0, -XMVectorGetY(origin) },
			DirectX::XMVECTOR{ 0, 0, 1, -XMVectorGetZ(origin) },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		DirectX::XMMATRIX view(
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(right), DirectX::XMVectorGetY(right), DirectX::XMVectorGetZ(right), 0 },
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(up), DirectX::XMVectorGetY(up), DirectX::XMVectorGetZ(up), 0 },
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(fwd), DirectX::XMVectorGetY(fwd), DirectX::XMVectorGetZ(fwd), 0 },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);


		DirectX::XMMATRIX project(
			DirectX::XMVECTOR{ 1 / w, 0, 0, 0 },
			DirectX::XMVECTOR{ 0, 1 / h, 0, 0 },
			DirectX::XMVECTOR{ 0, 0, farPlane / (farPlane - nearPlane), -farPlane * nearPlane / (farPlane - nearPlane) },
			DirectX::XMVECTOR{ 0, 0, 1, 0 }
		);


		DirectX::XMMATRIX mvp = project * view * translate;
		mvp = DirectX::XMMatrixTranspose(mvp);

		return mvp;
	}

	DirectX::XMMATRIX GetTransformMatrix(
		const DirectX::XMVECTOR& origin,
		const DirectX::XMVECTOR& target,
		const DirectX::XMVECTOR& frustrumSettings)
	{
		using namespace DirectX;

		float eps = 0.0000001f;

		XMVECTOR fwd = target - origin;

		fwd = XMVector3Normalize(fwd);

		XMVECTOR right, up;
		{
			XMVECTOR tmp{ 0, 1, 0, 0 };

			right = XMVector3Cross(tmp, fwd);
			if (XMVectorGetX(XMVector3LengthSq(right)) < eps)
			{
				right = XMVECTOR{ 1, 0, 0, 0 };
			}

			right = XMVector3Normalize(right);
			up = XMVector3Cross(fwd, right);

			up = XMVector3Normalize(up);
		}

		XMMATRIX mvp = GetTransformMatrixFromVectors(
			origin,
			target,
			right,
			up,
			frustrumSettings);

		return mvp;
	}

	DirectX::XMVECTOR FindPerp(const DirectX::XMVECTOR& vector)
	{
		using namespace DirectX;

		XMVECTOR x{ 1, 0, 0, 0 };
		XMVECTOR y{ 0, 1, 0, 0 };

		XMVECTOR res = XMVector3Cross(vector, x);
		XMVECTOR tmp = XMVector3Dot(res, res);

		if (XMVectorGetX(tmp) > 0)
		{
			return XMVector3Normalize(res);
		}
		
		res = XMVector3Cross(vector, y);
		return XMVector3Normalize(res);
	}

	DirectX::XMVECTOR ProjectToPlane(const DirectX::XMVECTOR& vector, const DirectX::XMVECTOR& planeNormal)
	{
		using namespace DirectX;

		XMVECTOR normal = XMVector3Normalize(planeNormal);
		XMVECTOR perp1 = FindPerp(normal);
		XMVECTOR perp2 = XMVector3Cross(normal, perp1);

		float c1 = XMVectorGetX(XMVector3Dot(vector, perp1));
		float c2 = XMVectorGetX(XMVector3Dot(vector, perp2));

		return c1 * perp1 + c2 * perp2;
	}

	void FindProjectionOrtho(
		const DirectX::XMVECTOR& direction,
		DirectX::XMMATRIX& matrix)
	{
		using namespace DirectX;
		using namespace rendering;

		float eps = 0.000001f;

		XMVECTOR camFwd = m_camera->GetTarget() - m_camera->GetPosition();
		camFwd = XMVector3Normalize(camFwd);

		XMVECTOR up, right, fwd;
		fwd = XMVector3Normalize(direction);
		{
			up = camFwd;
			right = XMVector3Cross(up, direction);

			{
				XMVECTOR l = XMVector3LengthSq(right);
				if (XMVectorGetX(l) < eps)
				{
					up = XMVectorSet(0, 1, 0, 0);
					right = XMVector3Cross(up, direction);
				}

				l = XMVector3LengthSq(right);
				if (XMVectorGetX(l) < eps)
				{
					up = XMVectorSet(1, 0, 0, 0);
					right = XMVector3Cross(up, direction);
				}
			}

			right = XMVector3Normalize(right);
			up = XMVector3Cross(fwd, right);
			up = XMVector3Normalize(up);
		}

		XMMATRIX viewRaw(
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(right), DirectX::XMVectorGetY(right), DirectX::XMVectorGetZ(right), 0 },
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(up), DirectX::XMVectorGetY(up), DirectX::XMVectorGetZ(up), 0 },
			DirectX::XMVECTOR{ DirectX::XMVectorGetX(fwd), DirectX::XMVectorGetY(fwd), DirectX::XMVectorGetZ(fwd), 0 },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		XMMATRIX view = XMMatrixTranspose(viewRaw);

		std::list<XMVECTOR> corners;
		m_camera->GetFrustrumCorners(corners);

		XMVECTOR minPoint = XMVectorSet(0, 0, 0, 0);
		XMVECTOR maxPoint = XMVectorSet(0, 0, 0, 0);

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			XMVECTOR cur = XMVector4Transform(*it, view);
			minPoint = XMVectorMin(cur, minPoint);
			maxPoint = XMVectorMax(cur, maxPoint);
		}

		XMVECTOR origin = (minPoint + maxPoint) / 2;
		XMVECTOR extents = maxPoint - origin;
		origin = XMVectorGetZ(minPoint) * fwd;

		float maxExtents = max(XMVectorGetX(extents), XMVectorGetY(extents));

		float depth = XMVectorGetZ(maxPoint) - XMVectorGetZ(minPoint);
		
		DirectX::XMMATRIX translate(
			DirectX::XMVECTOR{ 1, 0, 0, -XMVectorGetX(origin) },
			DirectX::XMVECTOR{ 0, 1, 0, -XMVectorGetY(origin) },
			DirectX::XMVECTOR{ 0, 0, 1, -XMVectorGetZ(origin) },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		DirectX::XMMATRIX scale(
			DirectX::XMVECTOR{ 1 / maxExtents, 0, 0, 0 },
			DirectX::XMVECTOR{ 0, 1 / maxExtents, 0, 0 },
			DirectX::XMVECTOR{ 0, 0, 1 / depth, 0 },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		matrix = XMMatrixMultiply(scale, XMMatrixMultiply(viewRaw, translate));
		matrix = XMMatrixTranspose(matrix);
	}

	DirectX::XMVECTOR FindShadowMapDirection(DirectX::XMVECTOR& origin, float& nearPlane, float& farPlane)
	{
		using namespace DirectX;
		using namespace rendering;

		float eps = 0.000001f;

		std::list<XMVECTOR> corners;
		m_camera->GetFrustrumCorners(corners);

		XMVECTOR camFwd = m_camera->GetTarget() - m_camera->GetPosition();
		camFwd = XMVector3Normalize(camFwd);

		XMVECTOR up = camFwd;
		
		XMVECTOR fst;
		XMVECTOR snd;

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			XMVECTOR tmp = ProjectToPlane(cur, up);

			XMVECTOR dot = XMVector3Dot(tmp, tmp);
			if (XMVectorGetX(dot) > 0)
			{
				fst = snd = XMVector3Normalize(tmp);
			}
		}

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			{
				XMVECTOR cross = XMVector3Cross(cur, fst);
				XMVECTOR dot = XMVector3Dot(up, cross);

				if (XMVectorGetX(dot) > 0) 
				{
					fst = XMVector3Normalize(ProjectToPlane(cur, up));
				}
			}

			{
				XMVECTOR cross = XMVector3Cross(cur, snd);
				XMVECTOR dot = XMVector3Dot(up, cross);

				if (XMVectorGetX(dot) < 0)
				{
					snd = XMVector3Normalize(ProjectToPlane(cur, up));
				}
			}
		}

		XMVECTOR horizontalDir = (fst + snd) / 2;
		horizontalDir = XMVector3Normalize(horizontalDir);

		up = XMVector3Cross(up, horizontalDir);
		up = XMVector3Normalize(up);

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			XMVECTOR tmp = ProjectToPlane(cur, up);

			XMVECTOR dot = XMVector3Dot(tmp, tmp);
			if (XMVectorGetX(dot) > 0)
			{
				fst = snd = XMVector3Normalize(tmp);
			}
		}

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			{
				XMVECTOR cross = XMVector3Cross(cur, fst);
				XMVECTOR dot = XMVector3Dot(up, cross);

				if (XMVectorGetX(dot) > 0)
				{
					fst = XMVector3Normalize(ProjectToPlane(cur, up));
				}
			}

			{
				XMVECTOR cross = XMVector3Cross(cur, snd);
				XMVECTOR dot = XMVector3Dot(up, cross);

				if (XMVectorGetX(dot) < 0)
				{
					snd = XMVector3Normalize(ProjectToPlane(cur, up));
				}
			}
		}

		XMVECTOR verticalDir = (fst + snd) / 2;
		verticalDir = XMVector3Normalize(verticalDir);

		farPlane = -1;
		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			XMVECTOR tmp = XMVector3Dot(cur, verticalDir);
			float d = XMVectorGetX(tmp);

			if (d > farPlane)
			{
				farPlane = d;
			}
		}

		nearPlane = farPlane;
		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			const XMVECTOR& cur = *it - origin;
			XMVECTOR tmp = XMVector3Dot(cur, verticalDir);
			float d = XMVectorGetX(tmp);

			if (d < nearPlane)
			{
				nearPlane = d;
			}
		}

		return verticalDir;
	}

	void GetShadowMapSettings(const rendering::Light& light, ShadowMapSettings& settings)
	{
		using namespace DirectX;
		using namespace rendering;

		XMMATRIX mvp;
		XMVECTOR lightDir = XMVectorSet(light.m_direction[0], light.m_direction[1], light.m_direction[2], 0);
		FindProjectionOrtho(lightDir, mvp);

		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mvp.r[r]);
			float y = DirectX::XMVectorGetY(mvp.r[r]);
			float z = DirectX::XMVectorGetZ(mvp.r[r]);
			float w = DirectX::XMVectorGetW(mvp.r[r]);

			settings.m_matrix[index++] = x;
			settings.m_matrix[index++] = y;
			settings.m_matrix[index++] = z;
			settings.m_matrix[index++] = w;
		}

		settings.m_position[0] = light.m_direction[0];
		settings.m_position[1] = light.m_direction[1];
		settings.m_position[2] = light.m_direction[2];
		settings.m_position[3] = 1;
	}
}

int rendering::LightsManager::m_shadowMapResolution = 2048;

rendering::LightsManager::LightsManager() :
	BaseObject(LightsManagerMeta::GetInstance())
{
	CacheObjects();

	Light l;
	l.m_direction[0] = 0;
	l.m_direction[1] = -1;
	l.m_direction[2] = 1;
	l.m_range = 300000;

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
			core::utils::RunSync(m_ctx.m_done);
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
			m_ctx.m_buffer = new DXBuffer(DXLightsBufferMeta::GetInstance());
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

	core::utils::RunSync(new CreateObjects(ctx));
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

		void Do()
		{
			m_ctx.m_texture = DXTexture::CreateDepthStencilTexture(DXShadowMapDSMeta::GetInstance(), m_shadowMapResolution, m_shadowMapResolution);

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

	core::utils::RunSync(new CreateTex(ctx));
}

void rendering::LightsManager::LoadShadowMapSettingsBuffer(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_manager = nullptr;
		DXBuffer* m_buffer = nullptr;
		DXHeap* m_heap = nullptr;

		jobs::Job* m_done = nullptr;
	};

	class CreateShadowMapUpdaters : public jobs::Job
	{
	public:
		CreateShadowMapUpdaters()
		{
		}

		void Do() override
		{
			new DXShadowMapUpdater();
			new DXShadowMapRDU();
		}
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
			m_ctx.m_manager->m_shadowMapSettingsBuffer = m_ctx.m_buffer;

			core::utils::RunSync(m_ctx.m_done);
			core::utils::RunSync(new CreateShadowMapUpdaters());
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

		void Do()
		{
			m_ctx.m_buffer = new DXBuffer(DXSMSettingsBufferMeta::GetInstance());
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

	Context ctx
	{
		this,
		nullptr,
		nullptr,
		done
	};

	core::utils::RunSync(new CreateBuffer(ctx));

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

		void Do()
		{
			m_ctx.m_texture = DXTexture::CreateRenderTargetTexture(DXShadowMapMeta::GetInstance(), m_shadowMapResolution, m_shadowMapResolution);
			
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

	core::utils::RunSync(new CreateTex(ctx));
}


void rendering::LightsManager::LoadShadowMap(jobs::Job* done)
{
	struct Context
	{
		LightsManager* m_lightsManager = nullptr;
		int m_itemsToLoad = 3;
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

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};


	Context* ctx = new Context();
	ctx->m_lightsManager = this;
	ctx->m_done = done;

	LoadShadowMapTex(new ItemDone(*ctx));
	LoadShadowMapDSTex(new ItemDone(*ctx));
	LoadShadowMapSettingsBuffer(new ItemDone(*ctx));
}

void rendering::LightsManager::UpdateShadowMapSettings()
{
	using namespace DirectX;
	float farPlane, nearPlane;

	void* data = m_shadowMapSettingsBuffer->Map();
	ShadowMapSettings* settingsData = static_cast<ShadowMapSettings*>(data);

	GetShadowMapSettings(m_lights.front(), *settingsData);

	m_shadowMapSettingsBuffer->Unmap();
	
	return;
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

rendering::DXBuffer* rendering::LightsManager::GetSMSettingsBuffer()
{
	return m_shadowMapSettingsBuffer;
}

