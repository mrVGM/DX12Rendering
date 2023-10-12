#pragma once

#include "BaseObjectMetaTag.h"

namespace settings
{
	class ReflectionEntryPointMetaTag : public BaseObjectMetaTag
	{
	public:
		static const ReflectionEntryPointMetaTag& GetInstance();
		ReflectionEntryPointMetaTag();
	};
}