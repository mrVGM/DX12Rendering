#include "SceneLoadedNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::SceneLoadedNotificationMeta m_meta;
}

rendering::SceneLoadedNotificationMeta::SceneLoadedNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::SceneLoadedNotificationMeta& rendering::SceneLoadedNotificationMeta::GetInstance()
{
	return m_meta;
}
