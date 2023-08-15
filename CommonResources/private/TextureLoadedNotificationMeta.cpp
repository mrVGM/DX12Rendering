#include "TextureLoadedNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::TextureLoadedNotificationMeta m_meta;
}

rendering::TextureLoadedNotificationMeta::TextureLoadedNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::TextureLoadedNotificationMeta& rendering::TextureLoadedNotificationMeta::GetInstance()
{
	return m_meta;
}
