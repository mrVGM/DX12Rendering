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
	};
}
