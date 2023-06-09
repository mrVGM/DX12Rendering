#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMaskFilterMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMaskFilterMaterialMeta& GetInstance();
		DXShadowMaskFilterMaterialMeta();
	};
}