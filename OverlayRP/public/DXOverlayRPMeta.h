#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class DXOverlayRPMeta : public BaseObjectMeta
	{
	public:
		static const DXOverlayRPMeta& GetInstance();
		DXOverlayRPMeta();
	};
}