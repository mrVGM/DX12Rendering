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
#include "resources/DXShadowSQMapMeta.h"
#include "resources/DXShadowMapFilterTexMeta.h"
#include "resources/DXShadowMapDSMeta.h"
#include "resources/DXShadowMaskMeta.h"

#include "updaters/DXShadowMapUpdater.h"
#include "updaters/DXShadowMapRDU.h"

#include "resources/DXSMSettingsBufferMeta.h"

#include "HelperMaterials/DXShadowMapMaterial.h"
#include "HelperMaterials/DXShadowMapFilterMaterial.h"

#include "HelperMaterials/DXShadowMaskMaterial.h"
#include "HelperMaterials/DXShadowMaskFilterMaterial.h"

#include "ShaderRepo.h"

#include "ICamera.h"

#include "DXDeferredMaterialMetaTag.h"

#include "NotificationReceiver.h"

#include "SceneLoadedNotificationMeta.h"
#include "MaterialResisteredNotificationMeta.h"

#include "DXMutableBuffer.h"

#include "CoreUtils.h"
#include "utils.h"

#include <DirectXMath.h>
#include <vector>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


namespace
{
	static const float eps = 0.00000000001f;

	rendering::ICamera* m_camera = nullptr;
	rendering::DXScene* m_scene = nullptr;
	rendering::ILightsManager* m_lightsManager = nullptr;
	rendering::Window* m_wnd = nullptr;
	rendering::DXMaterialRepo* m_materialRepo = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;

	rendering::DXTexture* m_gBuffPositionTex = nullptr;

	rendering::DXBuffer* m_renderTextureBuffer = nullptr;

	rendering::DXMaterial* m_shadowMaskMat = nullptr;
	std::vector<rendering::DXMaterial*> m_shadowMapIdentityFilterMat;
	std::vector<rendering::DXMaterial*> m_shadowMapGaussBlurFilterMat;

	rendering::DXMaterial* m_shadowMaskPCFFilterMat = nullptr;
	rendering::DXMaterial* m_shadowMaskDitherFilterMat = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_camera)
		{
			m_camera = cascaded::GetCamera();
		}

		if (!m_scene)
		{
			m_scene = cascaded::GetScene();
		}

		if (!m_lightsManager)
		{
			m_lightsManager = cascaded::GetLightsManager();
		}

		if (!m_wnd)
		{
			m_wnd = core::utils::GetWindow();
		}

		if (!m_materialRepo)
		{
			m_materialRepo = cascaded::GetMaterialRepo();
		}
		
		if (!m_commandQueue)
		{
			m_commandQueue = core::utils::GetCommandQueue();
		}

		if (!m_gBuffPositionTex)
		{
			m_gBuffPositionTex = cascaded::GetGBufferPositionTex();
		}

		if (!m_renderTextureBuffer)
		{
			m_renderTextureBuffer = cascaded::GetRenderTextureBuffer();
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
		float m_saparators[3];
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

		XMVECTOR up, right, fwd;
		fwd = XMVector3Normalize(direction);
		{
			up = XMVectorSet(0, 1, 0, 0);
			right = XMVector3Cross(up, direction);

			{
				XMVECTOR l = XMVector3LengthSq(right);
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
		float maxDist;
		m_camera->GetFrustrumCorners(corners, maxDist, nearPlane, farPlane);

		XMVECTOR minPoint = XMVectorSet(0, 0, 0, 0);
		XMVECTOR maxPoint = XMVectorSet(0, 0, 0, 0);

		{
			XMVECTOR cur = XMVector4Transform((*corners.begin()), view);
			cur /= XMVectorGetW(cur);
			minPoint = maxPoint = cur;
		}

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

			{
				XMVECTOR cur = XMVector4Transform((*corners.begin()), view);
				cur /= XMVectorGetW(cur);
				minPoint = XMVectorSetZ(minPoint, XMVectorGetZ(cur));
				maxPoint = XMVectorSetZ(maxPoint, XMVectorGetZ(cur));
			}

			for (int i = 0; i < _countof(sceneBBCorners); ++i)
			{
				XMVECTOR cur = XMVectorSetW(sceneBBCorners[i], 1);
				XMVECTOR proj = XMVector4Transform(cur, view);
				proj /= XMVectorGetW(proj);

				minPoint = XMVectorSetZ(minPoint, min(XMVectorGetZ(proj), XMVectorGetZ(minPoint)));
				maxPoint = XMVectorSetZ(maxPoint, max(XMVectorGetZ(proj), XMVectorGetZ(maxPoint)));
			}
		}

		origin = (minPoint + maxPoint) / 2;

		{
			float pixelSize = maxDist / CascadedSM::m_resolution;
			XMVECTOR tmp = origin;
			tmp /= pixelSize;

			tmp = XMVectorFloor(tmp);
			tmp *= pixelSize;

			origin = XMVectorSetX(origin, XMVectorGetX(tmp));
			origin = XMVectorSetY(origin, XMVectorGetY(tmp));
		}
		

		maxDist /= 2;
		{
			XMVECTOR extents = maxPoint - origin;
			float maxExtents = max(XMVectorGetX(extents), XMVectorGetY(extents));
			assert(maxExtents <= maxDist);
		}
		
		origin = XMVectorSetZ(origin, XMVectorGetZ(minPoint));
		origin = XMVectorSetW(origin, 1);
		origin = XMVector4Transform(origin, viewRaw);

		DirectX::XMMATRIX translate(
			DirectX::XMVECTOR{ 1, 0, 0, -XMVectorGetX(origin) },
			DirectX::XMVECTOR{ 0, 1, 0, -XMVectorGetY(origin) },
			DirectX::XMVECTOR{ 0, 0, 1, -XMVectorGetZ(origin) },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		float depth = XMVectorGetZ(maxPoint) - XMVectorGetZ(minPoint);

		DirectX::XMMATRIX scale(
			DirectX::XMVECTOR{ 1 / maxDist, 0, 0, 0 },
			DirectX::XMVECTOR{ 0, 1 / maxDist, 0, 0 },
			DirectX::XMVECTOR{ 0, 0, 1 / depth, 0 },
			DirectX::XMVECTOR{ 0, 0, 0, 1 }
		);

		matrix = XMMatrixMultiply(scale, XMMatrixMultiply(viewRaw, translate));
		matrix = XMMatrixTranspose(matrix);
	}


	class SceneDirty : public notifications::NotificationReceiver
	{
	public:
		SceneDirty(const BaseObjectMeta& meta) :
			notifications::NotificationReceiver(meta)
		{
		}

		void Notify() override
		{
			rendering::CascadedSM* sm = rendering::cascaded::GetCascadedSM();
			sm->SetListsDirty();
		}
	};
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
			m_ctx.m_cascadedSM->m_smTex.push_back(m_ctx.m_tex);

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

void rendering::CascadedSM::LoadSMTextures(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		int m_texturesLeft = 4;

		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_cascadedSM = this;
	ctx->m_done = done;

	class TextureDone : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		TextureDone(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_texturesLeft;
			if (m_ctx.m_texturesLeft > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);

			delete &m_ctx;
		}
	};

	LoadSMTexture(new TextureDone(*ctx));
	LoadSMTexture(new TextureDone(*ctx));
	LoadSMTexture(new TextureDone(*ctx));
	LoadSMTexture(new TextureDone(*ctx));
}

void rendering::CascadedSM::LoadSMFilterTexture(jobs::Job* done)
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
			m_ctx.m_cascadedSM->m_smFilterTex = m_ctx.m_tex;

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

			m_ctx.m_tex = DXTexture::CreateRenderTargetTexture(DXShadowMapFilterTexMeta::GetInstance(), size, size);

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

void rendering::CascadedSM::LoadShadowMaskTexture(jobs::Job* done)
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

	class PushTexture : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		PushTexture(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_cascadedSM->m_shadowMaskTex.push_back(m_ctx.m_tex);
			core::utils::RunSync(m_ctx.m_done);
		}
	};

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
			core::utils::RunSync(new PushTexture(m_ctx));
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
	for (auto it = m_smTex.begin(); it != m_smTex.end(); ++it)
	{
		textures.push_back(*it);
	}
	m_smDescriptorHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(
		DXDescriptorHeapMeta::GetInstance(), textures);
}

void rendering::CascadedSM::UpdateSMSettings()
{
	using namespace DirectX;
	const DirectionalLight& light = m_lightsManager->GetPrimaryDirectionalLight();

	XMMATRIX mat;
	XMVECTOR origin;

	ShadowMapSettings settings;

	float planes[] =
	{
		m_camera->GetNearPlane(),
		m_cascadeSeparators[0],
		m_cascadeSeparators[1],
		m_cascadeSeparators[2],
		m_camera->GetFarPlane(),
	};

	for (int i = 0; i < 4; ++i)
	{
		SingleSM& singleSMSettings = settings.m_sms[i];

		float nearPlane = m_camera->GetNearPlane();

		FindProjectionOrthographic(
			XMVectorSetW(light.m_direction, 0),
			planes[i],
			planes[i + 1],
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
		singleSMSettings.m_position[1] = XMVectorGetY(origin);
		singleSMSettings.m_position[2] = XMVectorGetZ(origin);
		singleSMSettings.m_position[3] = XMVectorGetW(origin);
	}

	settings.m_resolution = CascadedSM::m_resolution;
	settings.m_saparators[0] = m_cascadeSeparators[0];
	settings.m_saparators[1] = m_cascadeSeparators[1];
	settings.m_saparators[2] = m_cascadeSeparators[2];

	void* data = m_smSettingsBuffer->Map();
	ShadowMapSettings* shadowMapSettingsData = static_cast<ShadowMapSettings*>(data);
	*shadowMapSettingsData = settings;
	m_smSettingsBuffer->Unmap();
}

void rendering::CascadedSM::PreparePreSMRenderList()
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
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(0)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(1)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(2)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(3)->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),

			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_gBuffPositionTex->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		};
		m_preSMRenderList->ResourceBarrier(_countof(barrier), barrier);
	}

	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		m_preSMRenderList->ClearRenderTargetView(GetSMDescriptorHeap()->GetDescriptorHandle(0), clearColor, 0, nullptr);
		m_preSMRenderList->ClearRenderTargetView(GetSMDescriptorHeap()->GetDescriptorHandle(1), clearColor, 0, nullptr);
		m_preSMRenderList->ClearRenderTargetView(GetSMDescriptorHeap()->GetDescriptorHandle(2), clearColor, 0, nullptr);
		m_preSMRenderList->ClearRenderTargetView(GetSMDescriptorHeap()->GetDescriptorHandle(3), clearColor, 0, nullptr);
	}

	{
		DXDescriptorHeap* dsDescriptorHeap = GetDSDescriptorHeap();
		m_preSMRenderList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_preSMRenderList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(1), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_preSMRenderList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(2), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_preSMRenderList->ClearDepthStencilView(dsDescriptorHeap->GetDescriptorHandle(3), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	THROW_ERROR(
		m_preSMRenderList->Close(),
		"Can't close Command List!")

	m_preSMRenderListPrepared = true;
}

void rendering::CascadedSM::PreparePostSMRenderList()
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
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(0)->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(1)->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(2)->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(GetShadowMap(3)->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),

			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(m_gBuffPositionTex->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT),
		};
		m_postSMRenderList->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		m_postSMRenderList->Close(),
		"Can't close Command List!")

	m_postSMRenderListPrepared = true;
}

rendering::CascadedSM::CascadedSM() :
	ShadowMap(CascadedSMMeta::GetInstance())
{
	new SceneDirty(SceneLoadedNotificationMeta::GetInstance());
	new SceneDirty(MaterialResisteredNotificationMeta::GetInstance());

	CacheObjects();

	DXDevice* device = core::utils::GetDevice();

	{
		THROW_ERROR(
			device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
			"Can't create Command Allocator!")

		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_preSMRenderList)),
			"Can't reset Command List!")

		THROW_ERROR(
			m_preSMRenderList->Close(),
			"Can't close Command List!")

		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_postSMRenderList)),
			"Can't reset Command List!")

		THROW_ERROR(
			m_postSMRenderList->Close(),
			"Can't close Command List!")
	}
}

rendering::CascadedSM::~CascadedSM()
{
	if (m_commandListsCache)
	{
		delete[] m_commandListsCache;
	}
}


void rendering::CascadedSM::LoadResources(jobs::Job* done)
{
	struct Context
	{
		CascadedSM* m_cascadedSM = nullptr;
		int m_itemsToWaitFor = 7;

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

			m_ctx.m_cascadedSM->CreateDescriptorHeaps();

			core::utils::RunSync(m_ctx.m_done);

			m_shadowMaskMat = new DXShadowMaskMaterial(
				*shader_repo::GetDeferredRPVertexShader(),
				*shader_repo::GetShadowMaskPixelShader()
			);

			for (int i = 0; i < 4; ++i)
			{
				m_shadowMapGaussBlurFilterMat.push_back(new rendering::DXShadowMapFilterMaterial(
					*shader_repo::GetDeferredRPVertexShader(),
					*shader_repo::GetGaussBlurFilterPixelShader(),
					m_ctx.m_cascadedSM->GetShadowMap(i),
					m_ctx.m_cascadedSM->GetShadowMapFilterTex()
				));

				m_shadowMapIdentityFilterMat.push_back(new DXShadowMapFilterMaterial(
					*shader_repo::GetDeferredRPVertexShader(),
					*shader_repo::GetIdentityFilterPixelShader(),
					m_ctx.m_cascadedSM->GetShadowMapFilterTex(),
					m_ctx.m_cascadedSM->GetShadowMap(i)
				));
			}

			m_shadowMaskPCFFilterMat = new DXShadowMaskFilterMaterial(
				*shader_repo::GetDeferredRPVertexShader(),
				*shader_repo::GetShadowMaskPCFFilterPixelShader(),
				1
			);

			m_shadowMaskDitherFilterMat = new DXShadowMaskFilterMaterial(
				*shader_repo::GetDeferredRPVertexShader(),
				*shader_repo::GetShadowMaskDitherFilterPixelShader(),
				0
			);

			delete& m_ctx;

			new DXShadowMapUpdater();
			new DXShadowMapRDU();
		}
	};

	LoadSettingsBuffer(new ItemReady(*ctx));
	LoadDepthTextures(new ItemReady(*ctx));
	LoadSMTextures(new ItemReady(*ctx));
	LoadSMFilterTexture(new ItemReady(*ctx));
	LoadShadowMaskTexture(new ItemReady(*ctx));
	LoadShadowMaskTexture(new ItemReady(*ctx));
	LoadSMMaterials(new ItemReady(*ctx));
}

rendering::DXTexture* rendering::CascadedSM::GetShadowMap(int index)
{
	return m_smTex[index];
}

rendering::DXTexture* rendering::CascadedSM::GetShadowMapFilterTex()
{
	return m_smFilterTex;
}

rendering::DXTexture* rendering::CascadedSM::GetShadowMask(int index)
{
	return m_shadowMaskTex[index];
}

rendering::DXTexture* rendering::CascadedSM::GetShadowMask()
{
	return GetShadowMask(0);
}

void rendering::CascadedSM::RenderShadowMask()
{
	PreparePreSMRenderList();
	PreparePostSMRenderList();

	{
		ID3D12CommandList* ppCommandLists[] = { m_preSMRenderList.Get() };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	RenderScene();

	{
		ID3D12CommandList* ppCommandLists[] = { m_postSMRenderList.Get() };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	for (int i = 0; i < 4; ++i)
	{
		{
			DXBuffer* dummy = nullptr;
			ID3D12CommandList* commandList = m_shadowMapGaussBlurFilterMat[i]->GenerateCommandList(
				*m_renderTextureBuffer,
				*dummy, *dummy, 0, 0, 0);
			ID3D12CommandList* ppCommandLists[] = { commandList };
			m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}

		{
			DXBuffer* dummy = nullptr;
			ID3D12CommandList* commandList = m_shadowMapIdentityFilterMat[i]->GenerateCommandList(
				*m_renderTextureBuffer,
				*dummy, *dummy, 0, 0, 0);
			ID3D12CommandList* ppCommandLists[] = { commandList };
			m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
	}

	{
		DXBuffer* dummy = nullptr;
		ID3D12CommandList* commandList = m_shadowMaskMat->GenerateCommandList(
			*m_renderTextureBuffer,
			*dummy, *dummy, 0, 0, 0);
		ID3D12CommandList* ppCommandLists[] = { commandList };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	{
		DXBuffer* dummy = nullptr;
		ID3D12CommandList* commandList = m_shadowMaskPCFFilterMat->GenerateCommandList(
			*m_renderTextureBuffer,
			*dummy, *dummy, 0, 0, 0);
		ID3D12CommandList* ppCommandLists[] = { commandList };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	{
		DXBuffer* dummy = nullptr;
		ID3D12CommandList* commandList = m_shadowMaskDitherFilterMat->GenerateCommandList(
			*m_renderTextureBuffer,
			*dummy, *dummy, 0, 0, 0);
		ID3D12CommandList* ppCommandLists[] = { commandList };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
}

void rendering::CascadedSM::RenderScene()
{
	if (!m_listsDirty)
	{
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(m_numCommandLists, m_commandListsCache);
		return;
	}
	m_listsDirty = false;

	DXScene* scene = m_scene;
	DXMaterialRepo* repo = m_materialRepo;

	for (auto smMatIt = GetShadowMapMaterials().begin();
		smMatIt != GetShadowMapMaterials().end();
		++smMatIt)
	{
		(*smMatIt)->ResetCommandLists();
	}

	std::list<ID3D12CommandList*> deferredLists;
	for (int i = 0; i < scene->GetScenesCount(); ++i)
	{
		collada::ColladaScene& curColladaScene = *scene->m_colladaScenes[i];
		const DXScene::SceneResources& curSceneResources = scene->m_sceneResources[i];

		collada::Scene& s = curColladaScene.GetScene();

		for (auto it = s.m_objects.begin(); it != s.m_objects.end(); ++it)
		{
			collada::Object& obj = it->second;
			collada::Geometry& geo = s.m_geometries[obj.m_geometry];
			int instanceIndex = s.m_objectInstanceMap[it->first];

			const std::string& objectName = it->first;
			
			auto matOverrideIt = obj.m_materialOverrides.begin();

			for (auto it = geo.m_materials.begin(); it != geo.m_materials.end(); ++it)
			{
				const std::string matOverrideName = *matOverrideIt;
				++matOverrideIt;

				DXMaterial* mat = repo->GetMaterial(matOverrideName);

				const DXScene::GeometryResources& geometryResources = curSceneResources.m_geometryResources.find(obj.m_geometry)->second;
				DXBuffer* vertBuf = geometryResources.m_vertexBuffer;
				DXBuffer* indexBuf = geometryResources.m_indexBuffer;
				DXBuffer* instanceBuf = geometryResources.m_instanceBuffer->GetBuffer();

				if (!mat)
				{
					continue;
				}

				if (!mat->GetMeta().HasTag(DXDeferredMaterialMetaTag::GetInstance()))
				{
					continue;
				}

				for (auto smMatIt = GetShadowMapMaterials().begin();
					smMatIt != GetShadowMapMaterials().end();
					++smMatIt)
				{

					ID3D12CommandList* cl = (*smMatIt)->GenerateCommandList(
						*vertBuf,
						*indexBuf,
						*instanceBuf,
						(*it).indexOffset,
						(*it).indexCount,
						instanceIndex);

					deferredLists.push_back(cl);
				}
			}
		}
	}

	int numLists = deferredLists.size();
	if (m_numCommandLists < numLists)
	{
		if (m_numCommandLists)
		{
			delete[] m_commandListsCache;
		}
		m_commandListsCache = new ID3D12CommandList * [numLists];
		m_numCommandLists = numLists;
	}

	int index = 0;
	for (auto it = deferredLists.begin(); it != deferredLists.end(); ++it)
	{
		m_commandListsCache[index++] = *it;
	}

	m_commandQueue->GetCommandQueue()->ExecuteCommandLists(numLists, m_commandListsCache);
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

void rendering::CascadedSM::SetListsDirty()
{
	m_listsDirty = true;
}


#undef THROW_ERROR