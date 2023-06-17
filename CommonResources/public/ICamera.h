#pragma once

#include "BaseObject.h"
#include "BaseObjectMeta.h"

#include <DirectXMath.h>
#include <list>

namespace rendering
{
	class ICamera : public BaseObject
	{
	public:
		ICamera(const BaseObjectMeta& meta);
		virtual ~ICamera();

		virtual DirectX::XMVECTOR GetPosition() const = 0;
		virtual DirectX::XMVECTOR GetTarget() const = 0;

		virtual float GetNearPlane() const = 0;
		virtual float GetFarPlane() const = 0;

		virtual void GetFrustrumCorners(std::list<DirectX::XMVECTOR>& corners, float& maxDist, float nearPlane, float farPlane) = 0;

		virtual DirectX::XMMATRIX CamCoordinates() const = 0;
		virtual DirectX::XMMATRIX GetMVPMatrix(DirectX::XMVECTOR& right, DirectX::XMVECTOR& fwd, DirectX::XMVECTOR& up) const = 0;
	};

	namespace cam_utils
	{
		DirectX::XMMATRIX MakePerspectiveProjectionMatrix(
			const DirectX::XMVECTOR& right,
			const DirectX::XMVECTOR& fwd,
			const DirectX::XMVECTOR& up,
			const DirectX::XMVECTOR& origin,
			float nearPlane,
			float farPlane,
			float fovDegrees,
			float aspect
		);
	}
}
