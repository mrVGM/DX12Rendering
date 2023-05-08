#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDepthStencilDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXDepthStencilDescriptorHeapMeta& GetInstance();
		DXDepthStencilDescriptorHeapMeta();
	};
}