#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowSQMapMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowSQMapMeta& GetInstance();
		DXShadowSQMapMeta();
	};
}