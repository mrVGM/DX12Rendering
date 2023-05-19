#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDeviceMeta : public BaseObjectMeta
	{
	public:
		static const DXDeviceMeta& GetInstance();
		DXDeviceMeta();
	};
}