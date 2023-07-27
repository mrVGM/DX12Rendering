#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class LogJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const LogJobSystemMeta& GetInstance();
		LogJobSystemMeta();
	};
}