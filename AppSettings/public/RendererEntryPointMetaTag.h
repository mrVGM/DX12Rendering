#pragma once

#include "BaseObjectMetaTag.h"

namespace settings
{
	class RendererEntryPointMetaTag : public BaseObjectMetaTag
	{
	public:
		static const RendererEntryPointMetaTag& GetInstance();
		RendererEntryPointMetaTag();
	};
}