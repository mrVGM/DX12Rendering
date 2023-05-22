#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShadowMapDSDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXShadowMapDSDescriptorHeapMeta& GetInstance();
		DXShadowMapDSDescriptorHeapMeta();
	};
}