#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXLightsBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXLightsBufferMeta& GetInstance();
		DXLightsBufferMeta();
	};
}