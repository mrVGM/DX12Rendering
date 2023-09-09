#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class AsyncTickUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const AsyncTickUpdaterMeta& GetInstance();
		AsyncTickUpdaterMeta();
	};
}