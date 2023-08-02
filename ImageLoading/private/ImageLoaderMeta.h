#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class ImageLoaderMeta : public BaseObjectMeta
	{
	public:
		static const ImageLoaderMeta& GetInstance();
		ImageLoaderMeta();
	};
}