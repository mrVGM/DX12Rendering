#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapDSMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapDSMeta& GetInstance();
		DXShadowMapDSMeta();
	};
}