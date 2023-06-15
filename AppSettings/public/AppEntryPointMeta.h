#pragma once

#include "BaseObjectMeta.h"

namespace settings
{
	class AppEntryPointMeta : public BaseObjectMeta
	{
	public:
		static const AppEntryPointMeta& GetInstance();
		AppEntryPointMeta();
	};
}