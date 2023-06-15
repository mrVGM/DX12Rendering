#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMapUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapUpdaterMeta& GetInstance();
		DXShadowMapUpdaterMeta();
	};
}