#pragma once

#include "BaseObjectMeta.h"

namespace collada
{
	class ColladaAnimationMeta : public BaseObjectMeta
	{
	public:
		static const ColladaAnimationMeta& GetInstance();
		ColladaAnimationMeta();
	};
}