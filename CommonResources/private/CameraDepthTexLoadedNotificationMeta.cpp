#include "CameraDepthTexLoadedNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::CameraDepthTexLoadedNotificationMeta m_meta;
}

rendering::CameraDepthTexLoadedNotificationMeta::CameraDepthTexLoadedNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::CameraDepthTexLoadedNotificationMeta& rendering::CameraDepthTexLoadedNotificationMeta::GetInstance()
{
	return m_meta;
}
