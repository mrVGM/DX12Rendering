#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RenderDataUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const RenderDataUpdaterMeta& GetInstance();
		RenderDataUpdaterMeta();
	};
}