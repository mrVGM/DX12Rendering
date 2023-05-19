#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class ResidentHeapJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const ResidentHeapJobSystemMeta& GetInstance();
		ResidentHeapJobSystemMeta();
	};
}