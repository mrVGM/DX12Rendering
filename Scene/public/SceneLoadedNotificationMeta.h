#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class SceneLoadedNotificationMeta : public BaseObjectMeta
	{
	public:
		static const SceneLoadedNotificationMeta& GetInstance();
		SceneLoadedNotificationMeta();
	};
}