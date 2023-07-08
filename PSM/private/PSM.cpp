#include "PSM.h"

#include "PSMMeta.h"

#include "resources/DXShadowMapMeta.h"
#include "resources/DXShadowMapDSMeta.h"
#include "resources/DXShadowMaskMeta.h"
#include "resources/DXSMSettingsBufferMeta.h"

#include "resources/DXSMDescriptorHeapMeta.h"
#include "resources/DXSMDSDescriptorHeapMeta.h"

#include "HelperMaterials/DXShadowMapMaterial.h"
#include "HelperMaterials/DXShadowMaskMaterial.h"

#include "DXHeap.h"
#include "DXTexture.h"

#include "DXMutableBuffer.h"
#include "DXDescriptorHeap.h"

#include "updaters/DXShadowMapUpdater.h"

#include "DXDeferredMaterialMetaTag.h"

#include "ShaderRepo.h"
#include "CoreUtils.h"
#include "utils.h"

#include "AppSettings.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	rendering::Window* m_wnd = nullptr;
	rendering::DXDevice* m_device = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;
	rendering::ICamera* m_camera = nullptr;
	rendering::ILightsManager* m_lightsManager = nullptr;
	rendering::DXScene* m_scene = nullptr;
	rendering::DXMaterialRepo* m_materialRepo = nullptr;
	rendering::DXBuffer* m_renderTextureBuffer = nullptr;

	rendering::psm::DXShadowMapMaterial* m_shadowMapMaterial = nullptr;
	rendering::psm::DXShadowMaskMaterial* m_shadowMaskMaterial = nullptr;

	settings::AppSettings* m_appSettings = nullptr;

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

		if (!m_camera)
		{
			m_camera = rendering::psm::GetCamera();
		}

		if (!m_lightsManager)
		{
			m_lightsManager = rendering::psm::GetLightsManager();
		}

		if (!m_scene)
		{
			m_scene = rendering::psm::GetScene();
		}

		if (!m_materialRepo)
		{
			m_materialRepo = rendering::psm::GetMaterialRepo();
		}

		if (!m_renderTextureBuffer)
		{
			m_renderTextureBuffer = rendering::psm::GetRenderTextureBuffer();
		}

		if (!m_appSettings)
		{
			m_appSettings = settings::AppSettings::GetAppSettings();
		}
	}

	struct SMSettings
	{
		float m_lightPerspectiveMatrix[16];

		int m_straightLight = 1;
		int m_placeholder[3];
	};

	struct SMSettings1
	{
		float m_perspMatrix[16];
		float m_orthoMatrix[16];
	};

	DirectX::XMVECTOR OrientTowards(
		const DirectX::XMVECTOR& origin,
		const std::list<DirectX::XMVECTOR>& points,
		const DirectX::XMVECTOR& pole)
	{
		using namespace DirectX;

		const float eps = 0.000000001f;

		XMVECTOR X, Y;

		{
			X = XMVectorSet(1, 0, 0, 0);
			Y = XMVector3Cross(X, pole);

			if (XMVectorGetX(XMVector3LengthSq(Y)) < eps)
			{
				X = XMVectorSet(0, 1, 0, 0);
				Y = XMVector3Cross(X, pole);
			}

			Y = XMVector3Normalize(Y);
			X = XMVector3Cross(pole, Y);
			X = XMVector3Normalize(X);
		}

		std::list<XMVECTOR> ptsRelative;
		for (auto it = points.begin(); it != points.end(); ++it)
		{
			XMVECTOR cur = *it - origin;
			cur = XMVector3Dot(X, cur) * X + XMVector3Dot(Y, cur) * Y;

			ptsRelative.push_back(cur);
		}

		XMVECTOR minV, maxV;
		minV = maxV = ptsRelative.front();

		for (auto it = ptsRelative.begin(); it != ptsRelative.end(); ++it)
		{
			XMVECTOR crossMin = XMVector3Cross(minV, *it);
			XMVECTOR crossMax = XMVector3Cross(maxV, *it);

			XMVECTOR dotMin = XMVector3Dot(crossMin, pole);
			XMVECTOR dotMax = XMVector3Dot(crossMax, pole);

			if (XMVectorGetX(dotMin) < 0)
			{
				minV = *it;
			}

			if (XMVectorGetX(dotMin) > 0)
			{
				maxV = *it;
			}
		}

		minV = XMVector3Normalize(minV);
		maxV = XMVector3Normalize(maxV);

		XMVECTOR res = minV + maxV;
		res = XMVector3Normalize(res);
		return res;
	}

	void OrientVertically(
		DirectX::XMVECTOR& right,
		DirectX::XMVECTOR& fwd,
		DirectX::XMVECTOR& up,
		const DirectX::XMVECTOR& origin,
		const std::list<DirectX::XMVECTOR>& corners)
	{
		using namespace DirectX;

		float nearPlane, farPlane;

		{
			bool firstIteration = true;
			for (auto it = corners.begin(); it != corners.end(); ++it)
			{
				const XMVECTOR& cur = *it - origin;
				float d = XMVectorGetX(XMVector3Dot(cur, fwd));

				if (firstIteration)
				{
					firstIteration = false;
					nearPlane = farPlane = d;
					continue;
				}

				if (d < nearPlane)
				{
					nearPlane = d;
				}
				if (d > farPlane)
				{
					farPlane = d;
				}
			}
		}

		XMMATRIX mat = rendering::cam_utils::MakePerspectiveProjectionMatrix(
			right,
			fwd,
			up,
			origin,
			nearPlane,
			farPlane,
			60,
			1
		);

		XMVECTOR minYV, maxYV;
		{
			float minY, maxY;
			bool firstIteration = true;

			for (auto it = corners.begin(); it != corners.end(); ++it)
			{
				XMVECTOR cur = XMVector4Transform(*it, mat);
				cur /= XMVectorGetW(cur);
				float y = XMVectorGetY(cur);

				if (firstIteration)
				{
					firstIteration = false;
					minY = maxY = y;
					minYV = maxYV = *it;
					continue;
				}

				if (y < minY)
				{
					minY = y;
					minYV = *it;
				}
				if (y > maxY)
				{
					maxY = y;
					maxYV = *it;
				}
			}
		}

		minYV = XMVector3Dot(minYV, fwd) * fwd + XMVector3Dot(minYV, up) * up;
		minYV = XMVector3Normalize(minYV);

		maxYV = XMVector3Dot(maxYV, fwd) * fwd + XMVector3Dot(maxYV, up) * up;
		maxYV = XMVector3Normalize(maxYV);

		fwd = 0.5 * (minYV + maxYV);
		fwd = XMVector3Normalize(fwd);

		up = XMVector3Cross(fwd, right);
		up = XMVector3Normalize(up);

		right = XMVector3Cross(up, fwd);
		right = XMVector3Normalize(right);
	}

	struct LightPerspMatrix
	{
		DirectX::XMVECTOR m_right;
		DirectX::XMVECTOR m_fwd;
		DirectX::XMVECTOR m_up;
		std::list<DirectX::XMVECTOR> m_corners;

		DirectX::XMVECTOR m_bbMin;
		DirectX::XMVECTOR m_bbMax;

		DirectX::XMVECTOR m_origin;
		DirectX::XMMATRIX m_matrix;
		float m_near = 1;
		
		DirectX::XMVECTOR m_projectedMin;
		DirectX::XMVECTOR m_projectedMax;

		float GetDepth()
		{
			using namespace DirectX;
			XMVECTOR dV = m_bbMax - m_bbMin;
			float d = XMVectorGetZ(dV);

			return d;
		}

		void Init()
		{
			using namespace DirectX;

			{
				const XMVECTOR& cur = m_corners.front();

				XMVECTOR X = XMVector3Dot(cur, m_right);
				XMVECTOR Y = XMVector3Dot(cur, m_up);
				XMVECTOR Z = XMVector3Dot(cur, m_fwd);

				XMVECTOR tmp = XMVectorSet(
					XMVectorGetX(X),
					XMVectorGetX(Y),
					XMVectorGetX(Z),
					0
				);

				m_bbMin = m_bbMax = tmp;
			}

			for (auto it = m_corners.begin(); it != m_corners.end(); ++it)
			{
				const XMVECTOR& cur = *it;

				XMVECTOR X = XMVector3Dot(cur, m_right);
				XMVECTOR Y = XMVector3Dot(cur, m_up);
				XMVECTOR Z = XMVector3Dot(cur, m_fwd);

				XMVECTOR tmp = XMVectorSet(
					XMVectorGetX(X),
					XMVectorGetX(Y),
					XMVectorGetX(Z),
					0
				);

				m_bbMin = XMVectorMin(m_bbMin, tmp);
				m_bbMax = XMVectorMax(m_bbMax, tmp);
			}

			XMVECTOR origin = 0.5 * (m_bbMin + m_bbMax);
			origin = XMVectorSetZ(origin, XMVectorGetZ(m_bbMin) - m_near);

			origin = XMVectorGetX(origin) * m_right + XMVectorGetY(origin) * m_up + XMVectorGetZ(origin) * m_fwd;
			m_origin = origin;

			float d = GetDepth();
			m_matrix = rendering::cam_utils::MakePerspectiveProjectionMatrix(m_right, m_fwd, m_up, m_origin, m_near, m_near + d, 90, 1);
		}

		void Check()
		{
			using namespace DirectX;

			XMVECTOR pMin, pMax;

			{
				const XMVECTOR& cur = m_corners.front();
				XMVECTOR tmp = XMVector4Transform(cur, m_matrix);
				tmp /= XMVectorGetW(tmp);

				pMin = pMax = tmp;
			}

			for (auto it = m_corners.begin(); it != m_corners.end(); ++it)
			{
				const XMVECTOR& cur = *it;
				XMVECTOR tmp = XMVector4Transform(cur, m_matrix);
				tmp /= XMVectorGetW(tmp);

				pMin = XMVectorMin(pMin, tmp);
				pMax = XMVectorMax(pMax, tmp);
			}

			m_projectedMin = pMin;
			m_projectedMax = pMax;
		}

		void AdjustOrigin()
		{
			using namespace DirectX;

			Check();
			XMVECTOR adjustedProjectedOrigin = 0.5 * (m_projectedMin + m_projectedMax);
			adjustedProjectedOrigin = XMVectorSetZ(adjustedProjectedOrigin, 0);
			adjustedProjectedOrigin = XMVectorSetW(adjustedProjectedOrigin, m_near);

			XMVECTOR det;
			XMMATRIX inv = XMMatrixInverse(&det, m_matrix);

			XMVECTOR center = XMVector4Transform(adjustedProjectedOrigin, inv);
			
			m_origin = center - m_near * m_fwd;
			m_matrix = rendering::cam_utils::MakePerspectiveProjectionMatrix(m_right, m_fwd, m_up, m_origin, m_near, m_near + GetDepth(), 90, 1);
		}

		void GetFlatPlanePoints(std::list<DirectX::XMVECTOR>& outPoints)
		{
			using namespace DirectX;

			outPoints.clear();

			Check();

			XMVECTOR frontLeft = XMVectorSet(XMVectorGetX(m_projectedMin), 0, 0, m_near);
			XMVECTOR frontRight = XMVectorSet(XMVectorGetX(m_projectedMax), 0, 0, m_near);

			XMVECTOR det;
			XMMATRIX inv = XMMatrixInverse(&det, m_matrix);

			frontLeft = XMVector4Transform(frontLeft, inv);
			frontRight = XMVector4Transform(frontRight, inv);

			XMVECTOR left = XMVector3Normalize(frontLeft - m_origin);
			XMVECTOR right = XMVector3Normalize(frontRight - m_origin);

			XMVECTOR backLeft = m_origin + (m_near + GetDepth()) * left;
			XMVECTOR backRight = m_origin + (m_near + GetDepth()) * right;

			outPoints.push_back(frontLeft);
			outPoints.push_back(frontRight);
			outPoints.push_back(backLeft);
			outPoints.push_back(backRight);
		}

		void ExtendPointSceneBounds(
			const DirectX::XMVECTOR& point,
			rendering::DXScene* scene,
			DirectX::XMVECTOR& bound1,
			DirectX::XMVECTOR& bound2
		)
		{
			using namespace DirectX;

			XMVECTOR minPoint, maxPoint;
			scene->GetSceneBB(minPoint, maxPoint);

			float minY = XMVectorGetY(minPoint);
			float maxY = XMVectorGetY(maxPoint);

			float pY = XMVectorGetY(point);

			float d1 = minY - pY;
			float d2 = maxY - pY;

			float offset = XMVectorGetY(m_up);
			float c1 = d1 / offset;
			float c2 = d2 / offset;

			bound1 = point + c1 * m_up;
			bound2 = point + c2 * m_up;
		}
	};
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
			new DXShadowMapUpdater();
			m_ctx.m_psm->CreateDescriptorHeap();

			const shader_repo::ShaderSet& shadowMapShaderSet = shader_repo::GetShaderSetByName("psm_shadow_map");
			m_shadowMapMaterial = new DXShadowMapMaterial(*shadowMapShaderSet.m_vertexShader, *shadowMapShaderSet.m_pixelShader);

			const shader_repo::ShaderSet& shadowMaskShaderSet = shader_repo::GetShaderSetByName("psm_shadow_mask");
			m_shadowMaskMaterial = new DXShadowMaskMaterial(*shadowMaskShaderSet.m_vertexShader, *shadowMaskShaderSet.m_pixelShader);

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

rendering::DXTexture* rendering::psm::PSM::GetShadowMap(int index)
{
	return m_sm;
}

rendering::DXDescriptorHeap* rendering::psm::PSM::GetSMDescriptorHeap()
{
	return m_smDescriptorHeap;
}

rendering::DXDescriptorHeap* rendering::psm::PSM::GetSMDSDescriptorHeap()
{
	return m_smDSDescriptorHeap;
}

DirectX::XMVECTOR rendering::psm::PSM::GetLightPerspectiveOrigin()
{
	using namespace DirectX;

	XMVECTOR right, fwd, up;
	XMMATRIX mvp = m_camera->GetMVPMatrix(right, fwd, up);
	const rendering::DirectionalLight& light = m_lightsManager->GetPrimaryDirectionalLight();

	XMVECTOR lightDir = light.m_direction;
	lightDir = XMVectorSetW(lightDir, 0);

	XMVECTOR p1 = m_camera->GetPosition() + m_camera->GetNearPlane() * fwd;
	XMVECTOR p2 = p1 + lightDir;

	XMVECTOR p3 = m_camera->GetPosition() + m_camera->GetNearPlane() * fwd + up;
	XMVECTOR p4 = p3 + lightDir;

	p1 = XMVectorSetW(p1, 1);
	p2 = XMVectorSetW(p2, 1);
	p3 = XMVectorSetW(p3, 1);
	p4 = XMVectorSetW(p4, 1);

	p1 = XMVector4Transform(p1, mvp);
	p2 = XMVector4Transform(p2, mvp);
	p3 = XMVector4Transform(p3, mvp);
	p4 = XMVector4Transform(p4, mvp);

	p1 /= XMVectorGetW(p1);
	p2 /= XMVectorGetW(p2);
	p3 /= XMVectorGetW(p3);
	p4 /= XMVectorGetW(p4);

	XMVECTOR ray1 = p2 - p1;
	XMVECTOR ray2 = p4 - p3;

	XMVECTOR perp = XMVector3Cross(ray1, ray2);
	perp = XMVector3Cross(perp, ray1);

	XMVECTOR p3Offset = XMVector3Dot(p3, perp);
	XMVECTOR ray2Offset = XMVector3Dot(ray2, perp);

	float coef = XMVectorGetX(p3Offset) / XMVectorGetX(ray2Offset);
	XMVECTOR crossPoint = p3 - coef * ray2;

	return crossPoint;
}

void rendering::psm::PSM::UpdateSMSettings()
{
	using namespace DirectX;

	XMVECTOR lightPerspectiveOrigin = GetLightPerspectiveOrigin();
	XMVECTOR right, fwd, up;
	XMMATRIX mvp = m_camera->GetMVPMatrix(right, fwd, up);

	right = -right;
	fwd = -fwd;

	std::list<XMVECTOR> corners;
	corners.push_back(XMVectorSet(-1, -1, 0, 1));
	corners.push_back(XMVectorSet( 1, -1, 0, 1));
	corners.push_back(XMVectorSet( 1,  1, 0, 1));
	corners.push_back(XMVectorSet(-1,  1, 0, 1));

	corners.push_back(XMVectorSet(-1, -1, 1, 1));
	corners.push_back(XMVectorSet( 1, -1, 1, 1));
	corners.push_back(XMVectorSet( 1,  1, 1, 1));
	corners.push_back(XMVectorSet(-1,  1, 1, 1));

	XMVECTOR lpFwd = OrientTowards(lightPerspectiveOrigin, corners, up);
	XMVECTOR lpRight = XMVector3Cross(up, lpFwd);
	lpRight = XMVector3Normalize(lpRight);
	lpFwd = OrientTowards(lightPerspectiveOrigin, corners, lpRight);
	XMVECTOR lpUp = XMVector3Cross(lpFwd, lpRight);
	lpUp = XMVector3Normalize(lpUp);

	float nearPlane, farPlane;
	{
		bool firstIteration = true;
		for (auto it = corners.begin(); it != corners.end(); ++it)
		{
			XMVECTOR cur = *it - lightPerspectiveOrigin;
			XMVECTOR dot = XMVector3Dot(cur, lpFwd);
			float d = XMVectorGetX(dot);

			if (firstIteration)
			{
				firstIteration = false;
				nearPlane = farPlane = d;
				continue;
			}

			if (d < nearPlane)
			{
				nearPlane = d;
			}
			if (d > farPlane)
			{
				farPlane = d;
			}
		}
	}

	XMMATRIX mat = rendering::cam_utils::MakePerspectiveProjectionMatrix(lpRight, lpFwd, lpUp, lightPerspectiveOrigin, nearPlane, farPlane, 90, 1);

	float maxCoords = 0;
	for (auto it = corners.begin(); it != corners.end(); ++it)
	{
		XMVECTOR cur = XMVector4Transform(*it, mat);
		cur /= XMVectorGetW(cur);

		float tmp = max(abs(XMVectorGetX(cur)), abs(XMVectorGetY(cur)));
		maxCoords = max(maxCoords, tmp);
	}

	float fov = atan(maxCoords);
	fov *= 2;
	fov = XMConvertToDegrees(fov);


	mat = rendering::cam_utils::MakePerspectiveProjectionMatrix(lpRight, lpFwd, lpUp, lightPerspectiveOrigin, -nearPlane, nearPlane, fov, 1);

	SMSettings smSettings;
	{
		int index = 0;
		for (int r = 0; r < 4; ++r) {
			float x = DirectX::XMVectorGetX(mat.r[r]);
			float y = DirectX::XMVectorGetY(mat.r[r]);
			float z = DirectX::XMVectorGetZ(mat.r[r]);
			float w = DirectX::XMVectorGetW(mat.r[r]);

			smSettings.m_lightPerspectiveMatrix[index++] = x;
			smSettings.m_lightPerspectiveMatrix[index++] = y;
			smSettings.m_lightPerspectiveMatrix[index++] = z;
			smSettings.m_lightPerspectiveMatrix[index++] = w;
		}
	}

	void* data = m_settingsBuffer->GetUploadBuffer()->Map();
	SMSettings* settingsData = static_cast<SMSettings*>(data);
	*settingsData = smSettings;
	m_settingsBuffer->GetUploadBuffer()->Unmap();
	m_settingsBuffer->SetDirty();
}

void rendering::psm::PSM::UpdateSMSettings1()
{
	using namespace DirectX;

	const rendering::DirectionalLight& light = m_lightsManager->GetPrimaryDirectionalLight();

	XMVECTOR up = -light.m_direction;
	up = XMVector3Normalize(up);
	XMVECTOR fwd = m_camera->GetTarget() - m_camera->GetPosition();
	XMVECTOR right = XMVector3Cross(up, fwd);
	right = XMVector3Normalize(right);
	fwd = XMVector3Cross(right, up);

	std::list<XMVECTOR> corners;
	float maxDist;
	m_camera->GetFrustrumCorners(corners, maxDist, m_camera->GetNearPlane(), m_camera->GetFarPlane());


	LightPerspMatrix lpm;
	lpm.m_right = right;
	lpm.m_fwd = fwd;
	lpm.m_up = up;
	lpm.m_corners = corners;
	lpm.m_near = m_appSettings->GetSettings().m_psmNear;

	lpm.Init();
	lpm.Check();

	// TODO: Better convex hull calculation
	for (int i = 0; i < 20; ++i)
	{
		lpm.AdjustOrigin();
	}

	lpm.Check();

	std::list<XMVECTOR> flatPlanePoints;
	lpm.GetFlatPlanePoints(flatPlanePoints);

	for (auto it = flatPlanePoints.begin(); it != flatPlanePoints.end(); ++it)
	{
		XMVECTOR b1, b2;
		lpm.ExtendPointSceneBounds(*it, m_scene, b1, b2);

		bool t = true;
	}
}

void rendering::psm::PSM::CreateDescriptorHeap()
{
	{
		std::list<DXTexture*> textures;
		textures.push_back(m_sm);
		textures.push_back(m_shadowMask);

		m_smDescriptorHeap = DXDescriptorHeap::CreateRTVDescriptorHeap(DXSMDescriptorHeapMeta::GetInstance(), textures);
	}

	{
		std::list<DXTexture*> textures;
		textures.push_back(m_smDS);

		m_smDSDescriptorHeap = DXDescriptorHeap::CreateDSVDescriptorHeap(DXSMDSDescriptorHeapMeta::GetInstance(), textures);
	}
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

	m_preSMRenderList->ClearDepthStencilView(m_smDSDescriptorHeap->GetDescriptorHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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

	RenderScene();

	{
		ID3D12CommandList* ppCommandLists[] = { m_postSMRenderList.Get() };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	{
		DXBuffer* dummy = nullptr;
		ID3D12CommandList* commandList = m_shadowMaskMaterial->GenerateCommandList(
			*m_renderTextureBuffer,
			*dummy, *dummy, 0, 0, 0);
		ID3D12CommandList* ppCommandLists[] = { commandList };
		m_commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
}

rendering::DXTexture* rendering::psm::PSM::GetShadowMask()
{
	return m_shadowMask;
}


void rendering::psm::PSM::RenderScene()
{
	DXScene* scene = m_scene;
	DXMaterialRepo* repo = m_materialRepo;

	m_shadowMapMaterial->ResetCommandLists();

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

				ID3D12CommandList* cl = m_shadowMapMaterial->GenerateCommandList(
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

#undef THROW_ERROR