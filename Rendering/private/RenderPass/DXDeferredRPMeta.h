#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDeferredRPMeta : public BaseObjectMeta
	{
	public:
		static const DXDeferredRPMeta& GetInstance();
		DXDeferredRPMeta();
	};
}