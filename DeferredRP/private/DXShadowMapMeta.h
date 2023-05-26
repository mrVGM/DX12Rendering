#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapMeta& GetInstance();
		DXShadowMapMeta();
	};
}