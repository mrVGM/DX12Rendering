#pragma once

#include "BaseObjectMeta.h"

namespace scene_converter
{
	class SceneConverterEntryPointMeta : public BaseObjectMeta
	{
	public:
		static const SceneConverterEntryPointMeta& GetInstance();
		SceneConverterEntryPointMeta();
	};
}