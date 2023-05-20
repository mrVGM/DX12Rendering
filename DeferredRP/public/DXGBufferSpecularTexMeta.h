#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferSpecularTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferSpecularTexMeta& GetInstance();
			DXGBufferSpecularTexMeta();
		};
	}
}