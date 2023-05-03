#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXMaterialMeta& GetInstance();
		DXMaterialMeta();
	};
}