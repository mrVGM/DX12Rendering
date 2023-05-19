#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCopyBuffersMeta : public BaseObjectMeta
	{
	public:
		static const DXCopyBuffersMeta& GetInstance();
		DXCopyBuffersMeta();
	};
}