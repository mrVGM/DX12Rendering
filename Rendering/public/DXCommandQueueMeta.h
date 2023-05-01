#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCommandQueueMeta : public BaseObjectMeta
	{
	public:
		static const DXCommandQueueMeta& GetInstance();
		DXCommandQueueMeta();
	};
}