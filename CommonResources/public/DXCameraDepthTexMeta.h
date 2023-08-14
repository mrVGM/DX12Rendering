#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCameraDepthTexMeta : public BaseObjectMeta
	{
	public:
		static const DXCameraDepthTexMeta& GetInstance();
		DXCameraDepthTexMeta();
	};
}