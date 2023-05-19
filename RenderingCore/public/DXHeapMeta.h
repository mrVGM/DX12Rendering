#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXHeapMeta& GetInstance();
		DXHeapMeta();
	};
}