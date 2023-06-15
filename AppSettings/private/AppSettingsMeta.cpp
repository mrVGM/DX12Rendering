#include "AppSettingsMeta.h"

namespace
{
	settings::AppSettingsMeta m_meta;
}

settings::AppSettingsMeta::AppSettingsMeta() :
	BaseObjectMeta(nullptr)
{
}

const settings::AppSettingsMeta& settings::AppSettingsMeta::GetInstance()
{
	return m_meta;
}
