#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDisplaySMRPMeta : public BaseObjectMeta
	{
	public:
		static const DXDisplaySMRPMeta& GetInstance();
		DXDisplaySMRPMeta();
	};
}