#include "FontLoadedNotificationReceiverMeta.h"

#include "ImageLoadedNotificationMeta.h"

namespace
{
	rendering::overlay::FontLoadedNotificationReceiverMeta m_meta;
}

rendering::overlay::FontLoadedNotificationReceiverMeta::FontLoadedNotificationReceiverMeta() :
	BaseObjectMeta(&image_loading::ImageLoadedNotificationMeta::GetInstance())
{
}

const rendering::overlay::FontLoadedNotificationReceiverMeta& rendering::overlay::FontLoadedNotificationReceiverMeta::GetInstance()
{
	return m_meta;
}
