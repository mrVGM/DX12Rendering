#include "DXCameraUpdater.h"

#include "DXCameraUpdaterMeta.h"

#include "RenderUtils.h"

#include <corecrt_math_defines.h>

namespace 
{
	rendering::DXCamera* m_camera = nullptr;
	rendering::Window* m_window = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_camera)
		{
			m_camera = utils::GetCamera();
		}

		if (!m_window)
		{
			m_window = utils::GetWindow();
		}
	}

	void UpdateCameraPosition(double dt)
	{
		using namespace rendering;
		using namespace DirectX;

		const rendering::InputInfo& inputInfo = m_window->GetInputInfo();

		float right = 0;
		float forward = 0;
		float aimRight = 0;
		float aimUp = 0;

		for (std::set<WPARAM>::const_iterator it = inputInfo.m_keysDown.begin(); it != inputInfo.m_keysDown.end(); ++it) {
			WPARAM x = *it;
			if (x == 65) {
				right = -1;
			}
			if (x == 68) {
				right = 1;
			}

			if (x == 87) {
				forward = 1;
			}
			if (x == 83) {
				forward = -1;
			}

			if (x == 37) {
				aimRight = 1;
			}
			if (x == 39) {
				aimRight = -1;
			}
			if (x == 38) {
				aimUp = 1;
			}
			if (x == 40) {
				aimUp = -1;
			}
		}

		RECT rect;
		GetWindowRect(m_window->m_hwnd, &rect);

		if (inputInfo.m_rightMouseButtonDown && !m_camera->m_aiming) {
			m_camera->m_cursorRelativePos[0] = inputInfo.m_mouseMovement[0];
			m_camera->m_cursorRelativePos[1] = inputInfo.m_mouseMovement[1];
			m_camera->m_anglesCache[0] = m_camera->m_azimuth;
			m_camera->m_anglesCache[1] = m_camera->m_altitude;

			ClipCursor(&rect);
			ShowCursor(false);
		}

		if (!inputInfo.m_rightMouseButtonDown && m_camera->m_aiming) {
			ClipCursor(nullptr);
			ShowCursor(true);
		}


		double m_mouseAngleSpeed = 0.1;
		double m_angleSpeed = 80;
		double m_moveSpeed = 0.3;

		m_camera->m_aiming = inputInfo.m_rightMouseButtonDown;
		if (m_camera->m_aiming) {
			SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);

			m_camera->m_azimuth = -m_mouseAngleSpeed * (inputInfo.m_mouseMovement[0] - m_camera->m_cursorRelativePos[0]) + m_camera->m_anglesCache[0];
			while (m_camera->m_azimuth >= 360) {
				m_camera->m_azimuth -= 360;
			}
			while (m_camera->m_azimuth < 0) {
				m_camera->m_azimuth += 360;
			}

			m_camera->m_altitude = -m_mouseAngleSpeed * (inputInfo.m_mouseMovement[1] - m_camera->m_cursorRelativePos[1]) + m_camera->m_anglesCache[1];
			if (m_camera->m_altitude > 80) {
				m_camera->m_altitude = 80;
			}

			if (m_camera->m_altitude < -80) {
				m_camera->m_altitude = -80;
			}
		}

		m_camera->m_azimuth += dt * m_angleSpeed * aimRight;
		while (m_camera->m_azimuth >= 360) {
			m_camera->m_azimuth -= 360;
		}
		while (m_camera->m_azimuth < 0) {
			m_camera->m_azimuth += 360;
		}

		m_camera->m_altitude += dt * m_angleSpeed * aimUp;
		if (m_camera->m_altitude > 80) {
			m_camera->m_altitude = 80;
		}

		if (m_camera->m_altitude < -80) {
			m_camera->m_altitude = -80;
		}


		float azimuth = M_PI * m_camera->m_azimuth / 180.0;
		float altitude = M_PI * m_camera->m_altitude / 180.0;

		XMVECTOR fwdVector = XMVectorSet(cos(azimuth) * cos(altitude), sin(altitude), sin(azimuth) * cos(altitude), 0);
		XMVECTOR rightVector = XMVector3Cross(XMVectorSet(0, 1, 0, 0), fwdVector);
		rightVector = XMVector3Normalize(rightVector);

		XMVECTOR moveVector = XMVectorSet(right, 0, forward, 0);
		moveVector = XMVector3Normalize(moveVector);
		moveVector = m_moveSpeed * moveVector;
		moveVector = XMVectorAdd(XMVectorGetX(moveVector) * rightVector, XMVectorGetZ(moveVector) * fwdVector);

		m_camera->m_position = DirectX::XMVectorAdd(m_camera->m_position, moveVector);
		m_camera->m_target = DirectX::XMVectorAdd(m_camera->m_position, fwdVector);
	}

}

rendering::DXCameraUpdater::DXCameraUpdater() :
	TickUpdater(DXCameraUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXCameraUpdater::~DXCameraUpdater()
{
}

int rendering::DXCameraUpdater::GetPriority()
{
	return 0;
}

void rendering::DXCameraUpdater::Update(double dt)
{
	UpdateCameraPosition(dt);
	m_camera->UpdateCamBuffer();
}