#include "DXCamera.h"

#include "DXCameraMeta.h"

#include "d3dx12.h"
#include "Job.h"
#include "Window.h"
#include "DXHeap.h"
#include "DXBuffer.h"

#include "DXCameraRDU.h"

#include "DXCameraUpdater.h"

#include "RenderUtils.h"

#include <list>
#include <corecrt_math_defines.h>

DirectX::XMMATRIX rendering::DXCamera::GetMVPMatrix() const
{
	DirectX::XMVECTOR right, fwd, up;
	GetCoordinateVectors(right, fwd, up);
	
	float fovRad = DirectX::XMConvertToRadians(m_fov);

	float h = tan(fovRad / 2);
	float w = m_aspect * h;

	DirectX::XMMATRIX translate(
		DirectX::XMVECTOR{ 1, 0, 0, -DirectX::XMVectorGetX(m_position) },
		DirectX::XMVECTOR{ 0, 1, 0, -DirectX::XMVectorGetY(m_position) },
		DirectX::XMVECTOR{ 0, 0, 1, -DirectX::XMVectorGetZ(m_position) },
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
		DirectX::XMVECTOR{ 0, 0, m_farPlane / (m_farPlane - m_nearPlane), -m_farPlane * m_nearPlane / (m_farPlane - m_nearPlane)},
		DirectX::XMVECTOR{ 0, 0, 1, 0 }
	);


	DirectX::XMMATRIX mvp = project * view * translate;
	return DirectX::XMMatrixTranspose(mvp);
}

void rendering::DXCamera::GetCoordinateVectors(DirectX::XMVECTOR& right, DirectX::XMVECTOR& fwd, DirectX::XMVECTOR& up) const
{
	fwd = DirectX::XMVectorSubtract(m_target, m_position);

	up = DirectX::XMVectorSet(0, 1, 0, 1);
	right = DirectX::XMVector3Cross(up, fwd);
	up = DirectX::XMVector3Cross(fwd, right);

	fwd = DirectX::XMVector3Normalize(fwd);
	right = DirectX::XMVector3Normalize(right);
	up = DirectX::XMVector3Normalize(up);
}

void rendering::DXCamera::UpdateCamBuffer()
{
	using namespace DirectX;

	float matrixCoefs[20];
	DirectX::XMMATRIX mvp = GetMVPMatrix();

	int index = 0;
	for (int r = 0; r < 4; ++r) {
		float x = DirectX::XMVectorGetX(mvp.r[r]);
		float y = DirectX::XMVectorGetY(mvp.r[r]);
		float z = DirectX::XMVectorGetZ(mvp.r[r]);
		float w = DirectX::XMVectorGetW(mvp.r[r]);

		matrixCoefs[index++] = x;
		matrixCoefs[index++] = y;
		matrixCoefs[index++] = z;
		matrixCoefs[index++] = w;
	}

	matrixCoefs[index++] = XMVectorGetX(m_position);
	matrixCoefs[index++] = XMVectorGetY(m_position);
	matrixCoefs[index++] = XMVectorGetZ(m_position);
	matrixCoefs[index++] = 1;

	CD3DX12_RANGE readRange(0, 0);
	void* dst = nullptr;
	DXBuffer* camBuff = rendering::utils::GetCameraBuffer();

	if (FAILED(camBuff->GetBuffer()->Map(0, &readRange, &dst))) {
		return;
	}
	memcpy(dst, matrixCoefs, _countof(matrixCoefs) * sizeof(float));
	camBuff->GetBuffer()->Unmap(0, nullptr);
}

rendering::DXCamera::DXCamera() :
	ICamera(DXCameraMeta::GetInstance())
{
	using namespace DirectX;
	m_position = XMVectorSet(0, 0, -5, 1);
	m_target = XMVectorSet(0, 0, 0, 1);

	Window* wnd = utils::GetWindow();
	m_aspect = static_cast<float>(wnd->m_width) / wnd->m_height;

	class CreateCameraUpdater : public jobs::Job
	{
	public:
		void Do() override
		{
			new DXCameraUpdater();
		}
	};

	utils::RunSync(new CreateCameraUpdater());
}

rendering::DXCamera::~DXCamera()
{
}

void rendering::DXCamera::InitBuffer(jobs::Job* done)
{
	DXHeap* heap = new DXHeap();

	heap->SetHeapSize(256);
	heap->SetHeapType(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD);
	heap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
	heap->Create();
	
	struct JobContext
	{
		DXCamera* m_cam = nullptr;
		jobs::Job* m_done = nullptr;
		DXHeap* m_heap = nullptr;
	};

	class CreateRDU : public jobs::Job
	{
	public:
		CreateRDU()
		{
		}

		void Do()
		{
			new DXCameraRDU();
		}
	};

	class InitBufferJob : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		InitBufferJob(const JobContext& jobContext) :
			m_jobContext(jobContext)
		{
		}
		void Do() override
		{
			DXBuffer* camBuffer = rendering::utils::GetCameraBuffer();
			camBuffer->SetBufferSizeAndFlags(256, D3D12_RESOURCE_FLAG_NONE);
			camBuffer->SetBufferStride(256);

			camBuffer->Place(m_jobContext.m_heap, 0);

			utils::RunSync(m_jobContext.m_done);
			utils::RunSync(new CreateRDU());
		}
	};

	JobContext jobContext{ this, done, heap };

	heap->MakeResident(new InitBufferJob(jobContext));
}

void rendering::DXCamera::GetFrustrumCorners(std::list<DirectX::XMVECTOR>& corners, float nearPlane, float farPlane)
{
	using namespace DirectX;

	XMVECTOR right, fwd, up;
	GetCoordinateVectors(right, fwd, up);

	float fovRad = DirectX::XMConvertToRadians(m_fov);
	float h = tan(fovRad / 2);
	float w = m_aspect * h;

	XMVECTOR bottomLeft = fwd - h * up - w * right;
	XMVECTOR bottomRight = fwd - h * up + w * right;
	XMVECTOR topRight = fwd + h * up + w * right;
	XMVECTOR topLeft = fwd + h * up - w * right;

	corners.push_back(m_position + nearPlane * bottomLeft);
	corners.push_back(m_position + nearPlane * bottomRight);
	corners.push_back(m_position + nearPlane * topRight);
	corners.push_back(m_position + nearPlane * topLeft);

	corners.push_back(m_position + farPlane * bottomLeft);
	corners.push_back(m_position + farPlane * bottomRight);
	corners.push_back(m_position + farPlane * topRight);
	corners.push_back(m_position + farPlane * topLeft);

	for (auto it = corners.begin(); it != corners.end(); ++it)
	{
		XMVECTOR& cur = *it;
		cur = XMVectorSet(XMVectorGetX(cur), XMVectorGetY(cur), XMVectorGetZ(cur), 1);
	}
}

DirectX::XMVECTOR rendering::DXCamera::GetPosition() const
{
	return m_position;
}

DirectX::XMVECTOR rendering::DXCamera::GetTarget() const
{
	return m_target;
}

float rendering::DXCamera::GetNearPlane() const
{
	return m_nearPlane;
}

float rendering::DXCamera::GetFarPlane() const
{
	return m_farPlane;
}
