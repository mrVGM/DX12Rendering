#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXUnlitRPMeta : public BaseObjectMeta
	{
	public:
		static const DXUnlitRPMeta& GetInstance();
		DXUnlitRPMeta();
	};
}