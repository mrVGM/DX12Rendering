#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMaskMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMaskMaterialMeta& GetInstance();
		DXShadowMaskMaterialMeta();
	};
}