#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXUnlitMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXUnlitMaterialMeta& GetInstance();
		DXUnlitMaterialMeta();
	};
}