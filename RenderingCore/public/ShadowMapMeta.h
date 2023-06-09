#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class ShadowMapMeta : public BaseObjectMeta
	{
	public:
		static const ShadowMapMeta& GetInstance();
		ShadowMapMeta();
	};
}