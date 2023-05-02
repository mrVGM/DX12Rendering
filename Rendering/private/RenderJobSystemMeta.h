#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RenderJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const RenderJobSystemMeta& GetInstance();
		RenderJobSystemMeta();
	};
}