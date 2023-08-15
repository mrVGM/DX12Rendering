#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class TextureLoadedNotificationMeta : public BaseObjectMeta
	{
	public:
		static const TextureLoadedNotificationMeta& GetInstance();
		TextureLoadedNotificationMeta();
	};
}