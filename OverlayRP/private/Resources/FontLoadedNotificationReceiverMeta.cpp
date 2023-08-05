#include "FontLoadedNotificationReceiverMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::overlay::FontLoadedNotificationReceiverMeta m_meta;
}

rendering::overlay::FontLoadedNotificationReceiverMeta::FontLoadedNotificationReceiverMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::overlay::FontLoadedNotificationReceiverMeta& rendering::overlay::FontLoadedNotificationReceiverMeta::GetInstance()
{
	return m_meta;
}
