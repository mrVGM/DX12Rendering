#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class LoadJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const LoadJobSystemMeta& GetInstance();
		LoadJobSystemMeta();
	};
}