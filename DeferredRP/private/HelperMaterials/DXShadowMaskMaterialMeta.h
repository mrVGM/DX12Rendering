#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMaskMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMaskMaterialMeta& GetInstance();
		DXShadowMaskMaterialMeta();
	};
}