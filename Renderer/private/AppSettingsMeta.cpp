#include "AppSettingsMeta.h"

namespace
{
	rendering::AppSettingsMeta m_meta;
}

rendering::AppSettingsMeta::AppSettingsMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::AppSettingsMeta& rendering::AppSettingsMeta::GetInstance()
{
	return m_meta;
}
