#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXSwapChainMeta : public BaseObjectMeta
	{
	public:
		static const DXSwapChainMeta& GetInstance();
		DXSwapChainMeta();
	};
}