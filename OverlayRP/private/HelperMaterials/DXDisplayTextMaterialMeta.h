#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class DXDisplayTextMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXDisplayTextMaterialMeta& GetInstance();
		DXDisplayTextMaterialMeta();
	};
}