#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferNormalTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferNormalTexMeta& GetInstance();
			DXGBufferNormalTexMeta();
		};
	}
}