#pragma once

#include "BaseObject.h"

#include <queue>
#include <list>

#include <mutex>

namespace notifications
{
	class NotificationReceiver : public BaseObject
	{
	public:
		NotificationReceiver(const BaseObjectMeta& meta);
		virtual ~NotificationReceiver();

		virtual void Notify() = 0;
	};
}