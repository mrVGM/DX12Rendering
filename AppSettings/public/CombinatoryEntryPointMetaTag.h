#pragma once

#include "BaseObjectMetaTag.h"

namespace settings
{
	class CombinatoryEntryPointMetaTag : public BaseObjectMetaTag
	{
	public:
		static const CombinatoryEntryPointMetaTag& GetInstance();
		CombinatoryEntryPointMetaTag();
	};
}