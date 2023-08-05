#include "ImageLoadedNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::image_loading::ImageLoadedNotificationMeta m_meta;
}

rendering::image_loading::ImageLoadedNotificationMeta::ImageLoadedNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::image_loading::ImageLoadedNotificationMeta& rendering::image_loading::ImageLoadedNotificationMeta::GetInstance()
{
	return m_meta;
}
