#include "CascadedSM.h"

#include "CascadedSMMeta.h"

#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "DXTexture.h"

#include "CoreUtils.h"

#include "DXHeap.h"
#include "DXHeapMeta.h"

#include "DXDescriptorHeap.h"
#include "DXDescriptorHeapMeta.h"

#include "resources/DXShadowMapDSDescriptorHeapMeta.h"

#include "resources/DXShadowMapMeta.h"
#include "resources/DXShadowMapDSMeta.h"

#include "updaters/DXShadowMapUpdater.h"
#include "updaters/DXShadowMapRDU.h"

#include "resources/DXSMSettingsBufferMeta.h"

#include "HelperMaterials/DXShadowMapMaterial.h"

#include "ShaderRepo.h"

#include "ICamera.h"

#include "utils.h"

#include <DirectXMath.h>

namespace
{
	static const float eps = 0.00000000001f;

	rendering::ICamera* m_camera = nullptr;
	rendering::DXScene* m_scene = nullptr;
	rendering::LightsManager* m_lightsManager = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_camera)
		{
			m_camera = deferred::GetCamera();
		}

		if (!m_scene)
		{
			m_scene = deferred::GetScene();
		}

		if (!m_lightsManager)
		{
			m_lightsManager = deferred::GetLightsManager();
		}
	}

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

	struct SingleSM
	{
		float m_matrix[16];
		float m_inv[16];
		float m_position[4];
	};
	
	struct ShadowMapSettings
	{
		SingleSM m_sms[4];
		int m_resolution;
		float m_placeholder[3];
	};

	void FindProjectionOrthographic(
		const DirectX::XMVECTOR& direction,
		float nearPlane,
		float farPlane,
		DirectX::XMMATRIX& matrix,
		DirectX::XMVECTOR& origin)
	{
		using namespace DirectX;
		using namespace rendering;

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
		m_camera->GetFrustrumCorners(corners, nearPlane, farPlane);

		XMVECTOR minPoint = XMVectorSet(0, 0, 0, 0);
		XMVECTOR maxPoint = XMVectorSet(0, 0, 0, 0);

		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			XMVECTOR cur = XMVector4Transform(*it, view);
			cur /= XMVectorGetW(cur);
			minPoint = XMVectorMin(cur, minPoint);
			maxPoint = XMVectorMax(cur, maxPoint);
		}

		// TODO: Better near plane calculation
		{
			XMVECTOR minBB, maxBB;
			m_scene->GetSceneBB(minBB, maxBB);

			XMVECTOR sceneBBCorners[] =
			{
				minBB,
				XMVectorSetX(minBB, XMVectorGetX(maxBB)),
				XMVectorSetY(maxBB, XMVectorGetY(minBB)),
				XMVectorSetZ(minBB, XMVectorGetZ(maxBB)),

				maxBB,
				XMVectorSetX(maxBB, XMVectorGetX(minBB)),
				XMVectorSetY(minBB, XMVectorGetY(maxBB)),
				XMVectorSetZ(maxBB, XMVectorGetZ(minBB)),
			};

			for (int i = 0; i < _countof(sceneBBCorners); ++i)
			{
				XMVECTOR cur = XMVectorSetW(sceneBBCorners[i], 1);
				XMVECTOR proj = XMVector4Transform(cur, view);
				proj /= XMVectorGetW(proj);

				float minZ = min(XMVectorGetZ(proj), XMVectorGetZ(minPoint));
				minPoint = XMVectorSetZ(minPoint, minZ);
			}
		}

		origin = (minPoint + maxPoint) / 2;
		XMVECTOR extents = maxPoint - origin;
		origin = XMVectorGetZ(minPoint) * fwd;

		float maxExtents = max(XMVectorGetX(extents), XMVectorGetY(extents));

		DirectX::XMMATRIX translate(
			DirectX::XMVECTOR{ 1, 0, 0, -XMVectorGetX(origin) },
			DirectX::XMVECTOR{ 0, 1, 0, -XMVectorGetY(origin) },
			DirectX::XMVECTOR{ 0, 0, 1, -XMVectorGetZ(origin) },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		float depth = XMVectorGetZ(maxPoint) - XMVectorGetZ(minPoint);

		DirectX::XMMATRIX scale(
			DirectX::XMVECTOR{ 1 / maxExtents, 0, 0, 0 },
			DirectX::XMVECTOR{ 0, 1 / maxExtents, 0, 0 },
			DirectX::XMVECTOR{ 0, 0, 1 / depth, 0 },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		matrix = XMMatrixMultiply(scale, XMMatrixMultiply(viewRaw, translate));
		matrix = XMMatrixTranspose(matrix);
	}
}

const UINT rendering::CascadedSM::m_resolution = 2048;

void rendering::CascadedSM::LoadSettingsBuffer(jobs::Job* done)
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
			m_ctx.m_cascadedSM->m_smSettingsBuffer = m_ctx.m_buffer;

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
			UINT size = 3 * 256;
			UINT stride = size;

			m_ctx.m_buffer = new DXBuffer(DXSMSettingsBufferMeta::GetInstance());
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

void rendering::CascadedSM::LoadSMMaterials(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_self = nullptr;
		DXShadowMapMaterial* m_materials[4];

		int m_buffersLeftToLoad = _countof(m_materials);

		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_self = this;
	ctx->m_done = done;

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
			--m_ctx.m_buffersLeftToLoad;

			if (m_ctx.m_buffersLeftToLoad > 0)
			{
				return;
			}
			for (int i = 0; i < _countof(m_ctx.m_materials); ++i)
			{
				m_ctx.m_self->m_shadowMapMaterials.push_back(m_ctx.m_materials[i]);
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	for (int i = 0; i < _countof(ctx->m_materials); ++i)
	{
		ctx->m_materials[i] = new DXShadowMapMaterial(
			*shader_repo::GetShadowMapVertexShader(),
			*shader_repo::GetShadowMapPixelShader(),
			i);

		ctx->m_materials[i]->LoadBuffer(new BufferLoaded(*ctx));
	}
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

	std::list<DXTexture*> textures;
	textures.push_back(m_smTex);
	m_smDescriptorHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(
		DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::CascadedSM::UpdateSMSettings()
{
	using namespace DirectX;
	const Light& light = m_lightsManager->GetLight(0);

	XMMATRIX mat;
	XMVECTOR origin;

	ShadowMapSettings settings;

	{
		SingleSM& singleSMSettings = settings.m_sms[0];

		FindProjectionOrthographic(
			XMVectorSet(light.m_direction[0], light.m_direction[1], light.m_direction[2], 0),
			m_camera->GetNearPlane(),
			m_cascadeSeparators[0],
			mat,
			origin);

		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mat.r[r]);
			float y = DirectX::XMVectorGetY(mat.r[r]);
			float z = DirectX::XMVectorGetZ(mat.r[r]);
			float w = DirectX::XMVectorGetW(mat.r[r]);

			singleSMSettings.m_matrix[index++] = x;
			singleSMSettings.m_matrix[index++] = y;
			singleSMSettings.m_matrix[index++] = z;
			singleSMSettings.m_matrix[index++] = w;
		}

		XMVECTOR determinant;
		XMMATRIX inv = XMMatrixInverse(&determinant, mat);
		index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(inv.r[r]);
			float y = DirectX::XMVectorGetY(inv.r[r]);
			float z = DirectX::XMVectorGetZ(inv.r[r]);
			float w = DirectX::XMVectorGetW(inv.r[r]);

			singleSMSettings.m_inv[index++] = x;
			singleSMSettings.m_inv[index++] = y;
			singleSMSettings.m_inv[index++] = z;
			singleSMSettings.m_inv[index++] = w;
		}

		singleSMSettings.m_position[0] = XMVectorGetX(origin);
		singleSMSettings.m_position[1] = XMVectorGetX(origin);
		singleSMSettings.m_position[2] = XMVectorGetX(origin);
		singleSMSettings.m_position[3] = XMVectorGetX(origin);
	}

	{
		SingleSM& singleSMSettings = settings.m_sms[1];

		FindProjectionOrthographic(
			XMVectorSet(light.m_direction[0], light.m_direction[1], light.m_direction[2], 0),
			m_cascadeSeparators[0],
			m_cascadeSeparators[1],
			mat,
			origin);

		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mat.r[r]);
			float y = DirectX::XMVectorGetY(mat.r[r]);
			float z = DirectX::XMVectorGetZ(mat.r[r]);
			float w = DirectX::XMVectorGetW(mat.r[r]);

			singleSMSettings.m_matrix[index++] = x;
			singleSMSettings.m_matrix[index++] = y;
			singleSMSettings.m_matrix[index++] = z;
			singleSMSettings.m_matrix[index++] = w;
		}

		XMVECTOR determinant;
		XMMATRIX inv = XMMatrixInverse(&determinant, mat);
		index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(inv.r[r]);
			float y = DirectX::XMVectorGetY(inv.r[r]);
			float z = DirectX::XMVectorGetZ(inv.r[r]);
			float w = DirectX::XMVectorGetW(inv.r[r]);

			singleSMSettings.m_inv[index++] = x;
			singleSMSettings.m_inv[index++] = y;
			singleSMSettings.m_inv[index++] = z;
			singleSMSettings.m_inv[index++] = w;
		}

		singleSMSettings.m_position[0] = XMVectorGetX(origin);
		singleSMSettings.m_position[1] = XMVectorGetX(origin);
		singleSMSettings.m_position[2] = XMVectorGetX(origin);
		singleSMSettings.m_position[3] = XMVectorGetX(origin);
	}

	{
		SingleSM& singleSMSettings = settings.m_sms[2];

		FindProjectionOrthographic(
			XMVectorSet(light.m_direction[0], light.m_direction[1], light.m_direction[2], 0),
			m_cascadeSeparators[1],
			m_cascadeSeparators[2],
			mat,
			origin);

		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mat.r[r]);
			float y = DirectX::XMVectorGetY(mat.r[r]);
			float z = DirectX::XMVectorGetZ(mat.r[r]);
			float w = DirectX::XMVectorGetW(mat.r[r]);

			singleSMSettings.m_matrix[index++] = x;
			singleSMSettings.m_matrix[index++] = y;
			singleSMSettings.m_matrix[index++] = z;
			singleSMSettings.m_matrix[index++] = w;
		}

		XMVECTOR determinant;
		XMMATRIX inv = XMMatrixInverse(&determinant, mat);
		index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(inv.r[r]);
			float y = DirectX::XMVectorGetY(inv.r[r]);
			float z = DirectX::XMVectorGetZ(inv.r[r]);
			float w = DirectX::XMVectorGetW(inv.r[r]);

			singleSMSettings.m_inv[index++] = x;
			singleSMSettings.m_inv[index++] = y;
			singleSMSettings.m_inv[index++] = z;
			singleSMSettings.m_inv[index++] = w;
		}

		singleSMSettings.m_position[0] = XMVectorGetX(origin);
		singleSMSettings.m_position[1] = XMVectorGetX(origin);
		singleSMSettings.m_position[2] = XMVectorGetX(origin);
		singleSMSettings.m_position[3] = XMVectorGetX(origin);
	}

	{
		SingleSM& singleSMSettings = settings.m_sms[3];

		FindProjectionOrthographic(
			XMVectorSet(light.m_direction[0], light.m_direction[1], light.m_direction[2], 0),
			m_cascadeSeparators[2],
			m_camera->GetFarPlane(),
			mat,
			origin);

		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mat.r[r]);
			float y = DirectX::XMVectorGetY(mat.r[r]);
			float z = DirectX::XMVectorGetZ(mat.r[r]);
			float w = DirectX::XMVectorGetW(mat.r[r]);

			singleSMSettings.m_matrix[index++] = x;
			singleSMSettings.m_matrix[index++] = y;
			singleSMSettings.m_matrix[index++] = z;
			singleSMSettings.m_matrix[index++] = w;
		}

		XMVECTOR determinant;
		XMMATRIX inv = XMMatrixInverse(&determinant, mat);
		index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(inv.r[r]);
			float y = DirectX::XMVectorGetY(inv.r[r]);
			float z = DirectX::XMVectorGetZ(inv.r[r]);
			float w = DirectX::XMVectorGetW(inv.r[r]);

			singleSMSettings.m_inv[index++] = x;
			singleSMSettings.m_inv[index++] = y;
			singleSMSettings.m_inv[index++] = z;
			singleSMSettings.m_inv[index++] = w;
		}

		singleSMSettings.m_position[0] = XMVectorGetX(origin);
		singleSMSettings.m_position[1] = XMVectorGetX(origin);
		singleSMSettings.m_position[2] = XMVectorGetX(origin);
		singleSMSettings.m_position[3] = XMVectorGetX(origin);
	}

	settings.m_resolution = CascadedSM::m_resolution;

	void* data = m_smSettingsBuffer->Map();
	ShadowMapSettings* shadowMapSettingsData = static_cast<ShadowMapSettings*>(data);
	*shadowMapSettingsData = settings;
	m_smSettingsBuffer->Unmap();
}

rendering::CascadedSM::CascadedSM() :
	BaseObject(CascadedSMMeta::GetInstance())
{
	CacheObjects();
}

rendering::CascadedSM::~CascadedSM()
{
}


void rendering::CascadedSM::LoadResources(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		int m_itemsToWaitFor = 4;

		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_cascadedSM = this;
	ctx->m_done = done;

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

			m_ctx.m_cascadedSM->CreateDescriptorHeaps();

			core::utils::RunSync(m_ctx.m_done);
			delete& m_ctx;

			new DXShadowMapUpdater();
			new DXShadowMapRDU();
		}
	};

	LoadSettingsBuffer(new ItemReady(*ctx));
	LoadDepthTextures(new ItemReady(*ctx));
	LoadSMTexture(new ItemReady(*ctx));
	LoadSMMaterials(new ItemReady(*ctx));
}

rendering::DXTexture* rendering::CascadedSM::GetShadowMap()
{
	return m_smTex;
}

rendering::DXDescriptorHeap* rendering::CascadedSM::GetDSDescriptorHeap()
{
	return m_dsDescriptorHeap;
}

rendering::DXDescriptorHeap* rendering::CascadedSM::GetSMDescriptorHeap()
{
	return m_smDescriptorHeap;
}

rendering::DXBuffer* rendering::CascadedSM::GetSettingsBuffer()
{
	return m_smSettingsBuffer;
}

const std::list<rendering::DXMaterial*>& rendering::CascadedSM::GetShadowMapMaterials()
{
	return m_shadowMapMaterials;
}
