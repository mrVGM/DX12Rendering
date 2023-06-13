#include "DXCamera.h"

#include "DXCameraMeta.h"

#include "d3dx12.h"
#include "Job.h"
#include "Window.h"
#include "DXHeap.h"
#include "DXBuffer.h"

#include "DXCameraUpdater.h"

#include "RenderUtils.h"

#include "CoreUtils.h"

#include <list>
#include <corecrt_math_defines.h>

namespace
{
	struct CameraSettings
	{
		float m_matrix[16];
		float m_position[4];
		float m_fwd[4];
		float m_nearPlane;
		float m_farPlane;
		float m_fov;
		float m_aspect;

		int m_resolution[2];
		float placeholder[2];
	};

	rendering::Window* m_wnd = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_wnd)
		{
			m_wnd = core::utils::GetWindow();
		}
	}
}

DirectX::XMMATRIX rendering::DXCamera::GetMVPMatrix(DirectX::XMVECTOR& right, DirectX::XMVECTOR& fwd, DirectX::XMVECTOR& up) const
{
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

	CameraSettings camSettings;

	XMVECTOR right, fwd, up;
	DirectX::XMMATRIX mvp = GetMVPMatrix(right, fwd, up);

	int index = 0;
	for (int r = 0; r < 4; ++r) {
		float x = DirectX::XMVectorGetX(mvp.r[r]);
		float y = DirectX::XMVectorGetY(mvp.r[r]);
		float z = DirectX::XMVectorGetZ(mvp.r[r]);
		float w = DirectX::XMVectorGetW(mvp.r[r]);

		camSettings.m_matrix[index++] = x;
		camSettings.m_matrix[index++] = y;
		camSettings.m_matrix[index++] = z;
		camSettings.m_matrix[index++] = w;
	}

	camSettings.m_position[0] = XMVectorGetX(m_position);
	camSettings.m_position[1] = XMVectorGetY(m_position);
	camSettings.m_position[2] = XMVectorGetZ(m_position);
	camSettings.m_position[3] = 1;

	camSettings.m_fwd[0] = XMVectorGetX(fwd);
	camSettings.m_fwd[1] = XMVectorGetY(fwd);
	camSettings.m_fwd[2] = XMVectorGetZ(fwd);
	camSettings.m_fwd[3] = 1;

	camSettings.m_nearPlane = m_nearPlane;
	camSettings.m_farPlane = m_farPlane;
	camSettings.m_fov = m_fov;
	camSettings.m_aspect = m_aspect;

	camSettings.m_resolution[0] = m_wnd->m_width;
	camSettings.m_resolution[1] = m_wnd->m_height;

	DXMutableBuffer* camBuff = rendering::utils::GetCameraBuffer();
	void* data = camBuff->GetUploadBuffer()->Map();
	CameraSettings* camSettingsData = static_cast<CameraSettings*>(data);

	*camSettingsData = camSettings;

	camBuff->GetUploadBuffer()->Unmap();

	camBuff->SetDirty();
}

rendering::DXCamera::DXCamera() :
	ICamera(DXCameraMeta::GetInstance())
{
	using namespace DirectX;

	CacheObjects();

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
	struct JobContext
	{
		jobs::Job* m_done = nullptr;
	};
	JobContext jobContext{ done };

	class BufferLoaded : public jobs::Job
	{
	private:
		JobContext m_jobContext;
	public:
		BufferLoaded(const JobContext& jobContext) :
			m_jobContext(jobContext)
		{
		}
		void Do() override
		{
			utils::RunSync(m_jobContext.m_done);
		}
	};

	DXMutableBuffer* camBuffer = rendering::utils::GetCameraBuffer();
	camBuffer->Load(new BufferLoaded(jobContext));
}

void rendering::DXCamera::GetFrustrumCorners(std::list<DirectX::XMVECTOR>& corners, float& maxDist, float nearPlane, float farPlane)
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

	{
		XMVECTOR tmp = farPlane * topRight - nearPlane * bottomLeft;
		tmp = XMVector3Length(tmp);

		maxDist = 0;
		for (auto i = corners.begin(); i != corners.end(); ++i)
		{
			for (auto j = corners.begin(); j != corners.end(); ++j)
			{
				XMVECTOR tmp = (*i) - (*j);
				tmp = XMVector3Length(tmp);

				maxDist = max(maxDist, XMVectorGetX(tmp));
			}
		}
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
