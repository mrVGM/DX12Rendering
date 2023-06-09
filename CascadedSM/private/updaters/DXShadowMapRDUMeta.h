#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapRDUMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapRDUMeta& GetInstance();
		DXShadowMapRDUMeta();
	};
}