#pragma once

#include "BaseObjectMeta.h"

namespace animation
{
	class AnimatorUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const AnimatorUpdaterMeta& GetInstance();
		AnimatorUpdaterMeta();
	};
}