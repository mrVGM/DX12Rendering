#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXPostProcessMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXPostProcessMaterialMeta& GetInstance();
		DXPostProcessMaterialMeta();
	};
}