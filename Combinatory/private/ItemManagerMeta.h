#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class ItemManagerMeta : public BaseObjectMeta
	{
	public:
		static const ItemManagerMeta& GetInstance();
		ItemManagerMeta();
	};
}