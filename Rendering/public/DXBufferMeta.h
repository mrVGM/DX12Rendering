#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXBufferMeta& GetInstance();
		DXBufferMeta();
	};
}