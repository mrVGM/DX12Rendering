#pragma once

#include "BaseObjectMetaTag.h"

namespace rendering
{
	class DXDeferredMaterialMetaTag : public BaseObjectMetaTag
	{
	public:
		static const DXDeferredMaterialMetaTag& GetInstance();
		DXDeferredMaterialMetaTag();
	};
}