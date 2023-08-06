#include "FontLoadedNotificationReceiverMeta.h"

#include "FontAssetLoadedNotificationMeta.h"

namespace
{
	rendering::overlay::FontLoadedNotificationReceiverMeta m_meta;
}

rendering::overlay::FontLoadedNotificationReceiverMeta::FontLoadedNotificationReceiverMeta() :
	BaseObjectMeta(&image_loading::FontAssetLoadedNotificationMeta::GetInstance())
{
}

const rendering::overlay::FontLoadedNotificationReceiverMeta& rendering::overlay::FontLoadedNotificationReceiverMeta::GetInstance()
{
	return m_meta;
}
