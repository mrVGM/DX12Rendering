#pragma once

#include "BaseObjectMeta.h"

namespace notifications
{
	class NotificationReceiverMeta : public BaseObjectMeta
	{
	public:
		static const NotificationReceiverMeta& GetInstance();
		NotificationReceiverMeta();
	};
}