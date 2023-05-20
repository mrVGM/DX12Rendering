#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferDiffuseLitTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferDiffuseLitTexMeta& GetInstance();
			DXGBufferDiffuseLitTexMeta();
		};
	}
}