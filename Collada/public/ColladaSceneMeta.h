#pragma once

#include "BaseObjectMeta.h"

namespace collada
{
	class ColladaSceneMeta : public BaseObjectMeta
	{
	public:
		static const ColladaSceneMeta& GetInstance();
		ColladaSceneMeta();
	};
}