#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class SceneMaterialsSettingsMeta : public BaseObjectMeta
	{
	public:
		static const SceneMaterialsSettingsMeta& GetInstance();
		SceneMaterialsSettingsMeta();
	};
}