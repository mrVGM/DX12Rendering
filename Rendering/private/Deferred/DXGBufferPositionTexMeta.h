#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferPositionTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferPositionTexMeta& GetInstance();
			DXGBufferPositionTexMeta();
		};
	}
}