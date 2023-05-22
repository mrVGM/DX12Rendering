#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXUnlitErrorMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXUnlitErrorMaterialMeta& GetInstance();
		DXUnlitErrorMaterialMeta();
	};
}