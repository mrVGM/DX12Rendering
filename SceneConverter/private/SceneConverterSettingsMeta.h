#pragma once

#include "BaseObjectMeta.h"

namespace scene_converter
{
	class SceneConverterSettingsMeta : public BaseObjectMeta
	{
	public:
		static const SceneConverterSettingsMeta& GetInstance();
		SceneConverterSettingsMeta();
	};
}