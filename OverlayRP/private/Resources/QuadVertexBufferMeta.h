#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class QuadVertexBufferMeta : public BaseObjectMeta
	{
	public:
		static const QuadVertexBufferMeta& GetInstance();
		QuadVertexBufferMeta();
	};
}