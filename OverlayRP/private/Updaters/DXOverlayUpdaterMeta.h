#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class DXOverlayUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const DXOverlayUpdaterMeta& GetInstance();
		DXOverlayUpdaterMeta();
	};
}