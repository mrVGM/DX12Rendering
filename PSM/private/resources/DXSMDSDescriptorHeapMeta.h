#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXSMDSDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXSMDSDescriptorHeapMeta& GetInstance();
		DXSMDSDescriptorHeapMeta();
	};
}