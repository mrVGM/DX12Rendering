#include "FontAssetLoadedNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::image_loading::FontAssetLoadedNotificationMeta m_meta;
}

rendering::image_loading::FontAssetLoadedNotificationMeta::FontAssetLoadedNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::image_loading::FontAssetLoadedNotificationMeta& rendering::image_loading::FontAssetLoadedNotificationMeta::GetInstance()
{
	return m_meta;
}
