#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapRPMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapRPMeta& GetInstance();
		DXShadowMapRPMeta();
	};
}