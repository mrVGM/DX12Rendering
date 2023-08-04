#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class QuadIndexBufferMeta : public BaseObjectMeta
	{
	public:
		static const QuadIndexBufferMeta& GetInstance();
		QuadIndexBufferMeta();
	};
}