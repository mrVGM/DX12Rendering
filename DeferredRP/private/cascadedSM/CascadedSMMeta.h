#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CascadedSMMeta : public BaseObjectMeta
	{
	public:
		static const CascadedSMMeta& GetInstance();
		CascadedSMMeta();
	};
	
}