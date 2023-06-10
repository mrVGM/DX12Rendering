#include "MaterialResisteredNotificationMeta.h"

#include "NotificationReceiverMeta.h"

namespace
{
	rendering::MaterialResisteredNotificationMeta m_meta;
}

rendering::MaterialResisteredNotificationMeta::MaterialResisteredNotificationMeta() :
	BaseObjectMeta(&notifications::NotificationReceiverMeta::GetInstance())
{
}

const rendering::MaterialResisteredNotificationMeta& rendering::MaterialResisteredNotificationMeta::GetInstance()
{
	return m_meta;
}
