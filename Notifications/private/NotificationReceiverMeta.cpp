#include "NotificationReceiverMeta.h"

namespace
{
	notifications::NotificationReceiverMeta m_instance;
}

notifications::NotificationReceiverMeta::NotificationReceiverMeta() :
	BaseObjectMeta(nullptr)
{
}

const notifications::NotificationReceiverMeta& notifications::NotificationReceiverMeta::GetInstance()
{
	return m_instance;
}