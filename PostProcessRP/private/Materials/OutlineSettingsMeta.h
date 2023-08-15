#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class OutlineSettingsMeta : public BaseObjectMeta
	{
	public:
		static const OutlineSettingsMeta& GetInstance();
		OutlineSettingsMeta();
	};
}