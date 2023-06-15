#pragma once

#include "BaseObjectMetaTag.h"

namespace settings
{
	class SceneConverterEntryPointMetaTag : public BaseObjectMetaTag
	{
	public:
		static const SceneConverterEntryPointMetaTag& GetInstance();
		SceneConverterEntryPointMetaTag();
	};
}