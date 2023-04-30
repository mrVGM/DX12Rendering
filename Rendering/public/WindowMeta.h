#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class WindowMeta : public BaseObjectMeta
	{
	public:
		static const WindowMeta& GetInstance();
		WindowMeta();
	};
}