#pragma once

#include "BaseObjectMeta.h"

namespace frontend
{
	class FrontendManagerMeta : public BaseObjectMeta
	{
	public:
		static const FrontendManagerMeta& GetInstance();
		FrontendManagerMeta();
	};
}