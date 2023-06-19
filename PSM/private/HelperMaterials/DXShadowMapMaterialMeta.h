#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMapMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapMaterialMeta& GetInstance();
		DXShadowMapMaterialMeta();
	};
}