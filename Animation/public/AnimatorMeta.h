#pragma once

#include "BaseObjectMeta.h"

namespace animation
{
	class AnimatorMeta : public BaseObjectMeta
	{
	public:
		static const AnimatorMeta& GetInstance();
		AnimatorMeta();
	};
}