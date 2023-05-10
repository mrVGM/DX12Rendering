#include "Updater.h"

#include "UpdaterMeta.h"

#include "RenderUtils.h"

#include <corecrt_math_defines.h>

namespace
{
	class NotifyUpdater : public jobs::Job
	{
	private:
		rendering::Updater& m_updater;
	public:
		NotifyUpdater(rendering::Updater& updater) :
			m_updater(updater)
		{}
		void Do() override
		{
			--m_updater.m_updatesToWaitFor;
			m_updater.TryStartUpdate();
		}
	};

	void UpdateCameraPosition(double dt)
	{
		using namespace rendering;
		using namespace DirectX;

		Window* window = utils::GetWindow();
		const rendering::InputInfo& inputInfo = window->GetInputInfo();

		DXCamera* cam = utils::GetCamera();

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
		GetWindowRect(window->m_hwnd, &rect);

		if (inputInfo.m_rightMouseButtonDown && !cam->m_aiming) {
			cam->m_cursorRelativePos[0] = inputInfo.m_mouseMovement[0];
			cam->m_cursorRelativePos[1] = inputInfo.m_mouseMovement[1];
			cam->m_anglesCache[0] = cam->m_azimuth;
			cam->m_anglesCache[1] = cam->m_altitude;

			ClipCursor(&rect);
			ShowCursor(false);
		}

		if (!inputInfo.m_rightMouseButtonDown && cam->m_aiming) {
			ClipCursor(nullptr);
			ShowCursor(true);
		}

		cam->m_aiming = inputInfo.m_rightMouseButtonDown;
		if (cam->m_aiming) {
			SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
		}

		double m_angleSpeed = 80;
		double m_moveSpeed = 0.3;

		cam->m_azimuth += dt * m_angleSpeed * aimRight;
		while (cam->m_azimuth >= 360) {
			cam->m_azimuth -= 360;
		}
		while (cam->m_azimuth < 0) {
			cam->m_azimuth += 360;
		}

		cam->m_altitude += dt * m_angleSpeed * aimUp;
		if (cam->m_altitude > 80) {
			cam->m_altitude = 80;
		}

		if (cam->m_altitude < -80) {
			cam->m_altitude = -80;
		}


		float azimuth = M_PI * cam->m_azimuth / 180.0;
		float altitude = M_PI * cam->m_altitude / 180.0;

		XMVECTOR fwdVector = XMVectorSet(cos(azimuth) * cos(altitude), sin(altitude), sin(azimuth) * cos(altitude), 0);
		XMVECTOR rightVector = XMVector3Cross(XMVectorSet(0, 1, 0, 0), fwdVector);
		rightVector = XMVector3Normalize(rightVector);

		XMVECTOR moveVector = XMVectorSet(right, 0, forward, 0);
		moveVector = XMVector3Normalize(moveVector);
		moveVector = m_moveSpeed * moveVector;
		moveVector = XMVectorAdd(XMVectorGetX(moveVector) * rightVector, XMVectorGetZ(moveVector) * fwdVector);

		cam->m_position = DirectX::XMVectorAdd(cam->m_position, moveVector);
		cam->m_target = DirectX::XMVectorAdd(cam->m_position, fwdVector);

		cam->UpdateCamBuffer();
	}

	class UpdateCameraPositionJob : public jobs::Job
	{
	private:
		rendering::Updater& m_updater;
		double m_dt;
	public:
		UpdateCameraPositionJob(rendering::Updater& updater, double dt) :
			m_updater(updater),
			m_dt(dt)
		{
		}
		void Do() override
		{
			UpdateCameraPosition(m_dt);
			rendering::utils::RunSync(new NotifyUpdater(m_updater));
		}
	};
}

rendering::Updater::Updater() :
	BaseObject(rendering::UpdaterMeta::GetInstance())
{
}

rendering::Updater::~Updater()
{
}

void rendering::Updater::TryStartUpdate()
{
	if (m_updatesToWaitFor > 0)
	{
		return;
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	auto nowNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto lastTickNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_lastTick);
	long long deltaNN = nowNN.time_since_epoch().count() - lastTickNN.time_since_epoch().count();
	double dt = deltaNN / 1000000000.0;
	m_lastTick = now;

	StartUpdate(dt);
}

void rendering::Updater::StartUpdate(double dt)
{
	m_updatesToWaitFor = 2;

	DXRenderer* renderer = utils::GetRenderer();
	renderer->RenderFrame(new NotifyUpdater(*this));

	utils::RunAsync(new UpdateCameraPositionJob(*this, dt));
}

void rendering::Updater::Start()
{
	class RendererLoaded : public jobs::Job
	{
	private:
		Updater& m_updater;
	public:
		RendererLoaded(Updater& updater) :
			m_updater(updater)
		{
		}
		void Do() override
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			m_updater.m_lastTick = now;

			m_updater.StartUpdate(0);
		}
	};

	DXRenderer* renderer = utils::GetRenderer();
	renderer->LoadRPs(new RendererLoaded(*this));
}