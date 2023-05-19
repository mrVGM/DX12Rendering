#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXDescriptorHeapMeta& GetInstance();
		DXDescriptorHeapMeta();
	};
}