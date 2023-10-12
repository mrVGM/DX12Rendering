#pragma once

#include "BaseObjectMeta.h"

namespace reflection
{
	class ReflectionEntryPointMeta : public BaseObjectMeta
	{
	public:
		static const ReflectionEntryPointMeta& GetInstance();
		ReflectionEntryPointMeta();
	};
}