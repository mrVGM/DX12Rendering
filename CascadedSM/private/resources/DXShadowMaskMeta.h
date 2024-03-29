#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMaskMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMaskMeta& GetInstance();
		DXShadowMaskMeta();
	};
}