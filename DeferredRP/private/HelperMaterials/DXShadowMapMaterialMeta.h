#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapMaterialMeta& GetInstance();
		DXShadowMapMaterialMeta();
	};
}