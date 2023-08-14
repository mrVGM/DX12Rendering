#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CameraDepthTexLoadedNotificationMeta : public BaseObjectMeta
	{
	public:
		static const CameraDepthTexLoadedNotificationMeta& GetInstance();
		CameraDepthTexLoadedNotificationMeta();
	};
}