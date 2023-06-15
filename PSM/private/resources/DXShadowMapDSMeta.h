#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMapDSMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapDSMeta& GetInstance();
		DXShadowMapDSMeta();
	};
}