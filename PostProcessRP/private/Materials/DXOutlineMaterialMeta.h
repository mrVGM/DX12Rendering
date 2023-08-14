#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXOutlineMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXOutlineMaterialMeta& GetInstance();
		DXOutlineMaterialMeta();
	};
}