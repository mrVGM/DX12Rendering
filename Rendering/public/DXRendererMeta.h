#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXRendererMeta : public BaseObjectMeta
	{
	public:
		static const DXRendererMeta& GetInstance();
		DXRendererMeta();
	};
}