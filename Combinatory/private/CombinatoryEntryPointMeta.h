#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class CombinatoryEntryPointMeta : public BaseObjectMeta
	{
	public:
		static const CombinatoryEntryPointMeta& GetInstance();
		CombinatoryEntryPointMeta();
	};
}