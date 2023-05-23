#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCameraUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const DXCameraUpdaterMeta& GetInstance();
		DXCameraUpdaterMeta();
	};
}