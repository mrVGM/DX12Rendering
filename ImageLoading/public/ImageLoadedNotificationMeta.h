#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class ImageLoadedNotificationMeta : public BaseObjectMeta
	{
	public:
		static const ImageLoadedNotificationMeta& GetInstance();
		ImageLoadedNotificationMeta();
	};
}