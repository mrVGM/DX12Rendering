#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class ImageLoadingSettingsMeta : public BaseObjectMeta
	{
	public:
		static const ImageLoadingSettingsMeta& GetInstance();
		ImageLoadingSettingsMeta();
	};
}