#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXShadowMaskMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMaskMeta& GetInstance();
		DXShadowMaskMeta();
	};
}