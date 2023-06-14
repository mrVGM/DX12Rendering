#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class AppSettingsMeta : public BaseObjectMeta
	{
	public:
		static const AppSettingsMeta& GetInstance();
		AppSettingsMeta();
	};
}