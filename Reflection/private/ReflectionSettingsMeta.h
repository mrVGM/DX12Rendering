#pragma once

#include "BaseObjectMeta.h"

namespace reflection
{
	class ReflectionSettingsMeta : public BaseObjectMeta
	{
	public:
		static const ReflectionSettingsMeta& GetInstance();
		ReflectionSettingsMeta();
	};
}