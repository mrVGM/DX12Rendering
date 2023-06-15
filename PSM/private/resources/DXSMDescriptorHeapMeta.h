#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXSMDescriptorHeapMeta : public BaseObjectMeta
	{
	public:
		static const DXSMDescriptorHeapMeta& GetInstance();
		DXSMDescriptorHeapMeta();
	};
}