#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCameraBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXCameraBufferMeta& GetInstance();
		DXCameraBufferMeta();
	};
}