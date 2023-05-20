#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXPostLightsCalculationsMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXPostLightsCalculationsMaterialMeta& GetInstance();
		DXPostLightsCalculationsMaterialMeta();
	};
}