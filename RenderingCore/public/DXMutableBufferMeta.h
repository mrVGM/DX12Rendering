#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXMutableBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXMutableBufferMeta& GetInstance();
		DXMutableBufferMeta();
	};
}