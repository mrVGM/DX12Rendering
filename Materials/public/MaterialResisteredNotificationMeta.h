#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class MaterialResisteredNotificationMeta : public BaseObjectMeta
	{
	public:
		static const MaterialResisteredNotificationMeta& GetInstance();
		MaterialResisteredNotificationMeta();
	};
}