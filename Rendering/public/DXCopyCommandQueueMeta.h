#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCopyCommandQueueMeta : public BaseObjectMeta
	{
	public:
		static const DXCopyCommandQueueMeta& GetInstance();
		DXCopyCommandQueueMeta();
	};
}