#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CanvasVertexBufferMeta : public BaseObjectMeta
	{
	public:
		static const CanvasVertexBufferMeta& GetInstance();
		CanvasVertexBufferMeta();
	};
}