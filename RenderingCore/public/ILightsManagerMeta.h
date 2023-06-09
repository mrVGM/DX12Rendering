#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class ILightsManagerMeta : public BaseObjectMeta
	{
	public:
		static const ILightsManagerMeta& GetInstance();
		ILightsManagerMeta();
	};
}