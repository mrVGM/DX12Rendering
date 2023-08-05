#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class FontDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const FontDescriptorHeapMeta& GetInstance();
		FontDescriptorHeapMeta();
	};
}