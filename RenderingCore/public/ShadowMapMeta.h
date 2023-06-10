#pragma once

#include "BaseObjectMeta.h"

namespace rendering::shadow_mapping
{
	class ShadowMapMeta : public BaseObjectMeta
	{
	public:
		static const ShadowMapMeta& GetInstance();
		ShadowMapMeta();
	};
}