#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RenderPassMeta : public BaseObjectMeta
	{
	public:
		static const RenderPassMeta& GetInstance();
		RenderPassMeta();
	};
}