#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXClearDSTRPMeta : public BaseObjectMeta
	{
	public:
		static const DXClearDSTRPMeta& GetInstance();
		DXClearDSTRPMeta();
	};
}