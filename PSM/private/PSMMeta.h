#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class PSMMeta : public BaseObjectMeta
	{
	public:
		static const PSMMeta& GetInstance();
		PSMMeta();
	};
}