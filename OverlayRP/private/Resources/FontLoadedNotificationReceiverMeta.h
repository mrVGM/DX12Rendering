#pragma once

#include "BaseObjectMeta.h"

namespace rendering::overlay
{
	class FontLoadedNotificationReceiverMeta : public BaseObjectMeta
	{
	public:
		static const FontLoadedNotificationReceiverMeta& GetInstance();
		FontLoadedNotificationReceiverMeta();
	};
}