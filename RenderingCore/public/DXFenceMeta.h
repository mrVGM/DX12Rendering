#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXFenceMeta : public BaseObjectMeta
	{
	public:
		static const DXFenceMeta& GetInstance();
		DXFenceMeta();
	};
}