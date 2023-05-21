#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class TickUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const TickUpdaterMeta& GetInstance();
		TickUpdaterMeta();
	};
}