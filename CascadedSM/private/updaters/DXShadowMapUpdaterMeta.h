#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapUpdaterMeta& GetInstance();
		DXShadowMapUpdaterMeta();
	};
}