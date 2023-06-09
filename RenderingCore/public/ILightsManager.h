#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <DirectXMath.h>

namespace rendering
{
	struct DirectionalLight
	{
		DirectX::XMVECTOR m_direction;
	};

	class ILightsManager : public BaseObject
	{
	public:
		ILightsManager(const BaseObjectMeta& meta);
		virtual ~ILightsManager();

		virtual const DirectionalLight& GetPrimaryDirectionalLight() const = 0;
	};
}