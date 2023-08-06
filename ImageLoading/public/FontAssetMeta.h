#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class FontAssetMeta : public BaseObjectMeta
	{
	public:
		static const FontAssetMeta& GetInstance();
		FontAssetMeta();
	};
}