#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDisplaySMMaterialMeta : public BaseObjectMeta
	{
	public:
		static const DXDisplaySMMaterialMeta& GetInstance();
		DXDisplaySMMaterialMeta();
	};
}