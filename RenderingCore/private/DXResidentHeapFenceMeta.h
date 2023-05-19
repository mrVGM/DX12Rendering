#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXResidentHeapFenceMeta : public BaseObjectMeta
	{
	public:
		static const DXResidentHeapFenceMeta& GetInstance();
		DXResidentHeapFenceMeta();
	};
}