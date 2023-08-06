#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class FontAssetLoadedNotificationMeta : public BaseObjectMeta
	{
	public:
		static const FontAssetLoadedNotificationMeta& GetInstance();
		FontAssetLoadedNotificationMeta();
	};
}