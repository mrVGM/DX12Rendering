#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	namespace deferred
	{
		class DXGBufferDuffuseTexMeta : public BaseObjectMeta
		{
		public:
			static const DXGBufferDuffuseTexMeta& GetInstance();
			DXGBufferDuffuseTexMeta();
		};
	}
}