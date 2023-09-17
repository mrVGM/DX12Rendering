#pragma once

#include "BaseObjectMeta.h"

namespace frontend
{
	class FrontendJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const FrontendJobSystemMeta& GetInstance();
		FrontendJobSystemMeta();
	};
}