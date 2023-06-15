#pragma once

#include "BaseObjectMeta.h"

namespace settings
{
	class AppSettingsMeta : public BaseObjectMeta
	{
	public:
		static const AppSettingsMeta& GetInstance();
		AppSettingsMeta();
	};
}