#pragma once

#include "BaseObjectMeta.h"

namespace animation
{
	class AnimRepoMeta : public BaseObjectMeta
	{
	public:
		static const AnimRepoMeta& GetInstance();
		AnimRepoMeta();
	};
}