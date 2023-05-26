#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXLightsCalculationsMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXLightsCalculationsMaterialMeta& GetInstance();
		DXLightsCalculationsMaterialMeta();
	};
}