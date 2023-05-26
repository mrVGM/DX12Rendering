#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferAmbientLitTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferAmbientLitTexMeta& GetInstance();
			DXGBufferAmbientLitTexMeta();
		};
	}
}