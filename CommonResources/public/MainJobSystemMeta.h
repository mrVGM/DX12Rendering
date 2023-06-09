#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class MainJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const MainJobSystemMeta& GetInstance();
		MainJobSystemMeta();
	};
}