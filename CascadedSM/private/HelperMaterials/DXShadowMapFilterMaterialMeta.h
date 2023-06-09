#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapFilterMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapFilterMaterialMeta& GetInstance();
		DXShadowMapFilterMaterialMeta();
	};
}