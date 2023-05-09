#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDeferredMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXDeferredMaterialMeta& GetInstance();
		DXDeferredMaterialMeta();
	};
}