#pragma once

#include "BaseObjectMetaTag.h"

namespace rendering
{
	class DXUnlitMaterialMetaTag : public BaseObjectMetaTag
	{
	public:
		static const DXUnlitMaterialMetaTag& GetInstance();
		DXUnlitMaterialMetaTag();
	};
}