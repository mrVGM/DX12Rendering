#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXPostProcessRPMeta : public BaseObjectMeta
	{
	public:
		static const DXPostProcessRPMeta& GetInstance();
		DXPostProcessRPMeta();
	};
}