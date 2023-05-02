#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RenderFenceMeta : public BaseObjectMeta
	{
	public:
		static const RenderFenceMeta& GetInstance();
		RenderFenceMeta();
	};
}