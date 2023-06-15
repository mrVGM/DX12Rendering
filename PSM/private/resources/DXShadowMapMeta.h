#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMapMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapMeta& GetInstance();
		DXShadowMapMeta();
	};
}