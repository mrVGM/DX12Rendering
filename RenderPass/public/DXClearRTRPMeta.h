#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXClearRTRPMeta : public BaseObjectMeta
	{
	public:
		static const DXClearRTRPMeta& GetInstance();
		DXClearRTRPMeta();
	};
}