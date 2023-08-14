#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CanvasIndexBufferMeta : public BaseObjectMeta
	{
	public:
		static const CanvasIndexBufferMeta& GetInstance();
		CanvasIndexBufferMeta();
	};
}