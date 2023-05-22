#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class LightsManagerMeta : public BaseObjectMeta
	{
	public:
		static const LightsManagerMeta& GetInstance();
		LightsManagerMeta();
	};
}