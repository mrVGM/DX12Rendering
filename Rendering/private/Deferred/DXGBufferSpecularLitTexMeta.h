#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferSpecularLitTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferSpecularLitTexMeta& GetInstance();
			DXGBufferSpecularLitTexMeta();
		};
	}
}