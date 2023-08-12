#pragma once

#include "BaseObjectMeta.h"

namespace collada
{
	class SceneSettingsMeta : public BaseObjectMeta
	{
	public:
		static const SceneSettingsMeta& GetInstance();
		SceneSettingsMeta();
	};
}