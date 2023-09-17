#pragma once

#include "BaseObjectMeta.h"

namespace jobs
{
	class MainJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const MainJobSystemMeta& GetInstance();
		MainJobSystemMeta();
	};
}