#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXAnimUpdaterMeta : public BaseObjectMeta
	{
	public:
		static const DXAnimUpdaterMeta& GetInstance();
		DXAnimUpdaterMeta();
	};
}