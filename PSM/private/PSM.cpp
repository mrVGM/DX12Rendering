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

#include "updaters/DXShadowMapUpdater.h"

#include "CoreUtils.h"
#include "utils.h"

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
	}

	struct SMSettings
	{
		float m_lightPerspectiveMatrix[16];
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


	mat = rendering::cam_utils::MakePerspectiveProjectionMatrix(lpRight, lpFwd, lpUp, lightPerspectiveOrigin, nearPlane, farPlane, fov, 1);

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