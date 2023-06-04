#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapFilterTexMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapFilterTexMeta& GetInstance();
		DXShadowMapFilterTexMeta();
	};
}