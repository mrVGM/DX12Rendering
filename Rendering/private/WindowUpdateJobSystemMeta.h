#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class WindowUpdateJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const WindowUpdateJobSystemMeta& GetInstance();
		WindowUpdateJobSystemMeta();
	};
}