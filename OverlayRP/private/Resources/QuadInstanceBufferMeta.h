#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class QuadInstanceBufferMeta : public BaseObjectMeta
	{
	public:
		static const QuadInstanceBufferMeta& GetInstance();
		QuadInstanceBufferMeta();
	};
}