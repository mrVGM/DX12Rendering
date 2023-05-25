#pragma once

#include "ICamera.h"

#include "Job.h"
#include "JobSystem.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <string>

namespace rendering
{
	class DXCamera : public ICamera
	{
		float m_fov = 60;
		float m_aspect = 1;

		float m_nearPlane = 0.1;
		float m_farPlane = 100;

		DirectX::XMMATRIX GetMVPMatrix() const;
		void GetCoordinateVectors(DirectX::XMVECTOR& right, DirectX::XMVECTOR& fwd, DirectX::XMVECTOR& up) const;

	public:
		long m_cursorRelativePos[2] = { 0, 0 };
		float m_anglesCache[2] = { 0, 0 };
		bool m_aiming = false;
		int m_move[3] = { 0, 0, 0 };
		int m_aim[2] = { 0, 0 };

		float m_azimuth = 90;
		float m_altitude = 0;

		DirectX::XMVECTOR m_position;
		DirectX::XMVECTOR m_target;

		DXCamera();
		virtual ~DXCamera();

		void GetFrustrumCorners(std::list<DirectX::XMVECTOR>& corners) override;
		DirectX::XMVECTOR GetPosition() const override;
		DirectX::XMVECTOR GetTarget() const override;

		void InitBuffer(jobs::Job* done);
		void UpdateCamBuffer();
	};
}