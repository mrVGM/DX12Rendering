#include "SettingsReaderMeta.h"

namespace
{
	settings::SettingsReaderMeta m_meta;
}

settings::SettingsReaderMeta::SettingsReaderMeta() :
	BaseObjectMeta(nullptr)
{
}

const settings::SettingsReaderMeta& settings::SettingsReaderMeta::GetInstance()
{
	return m_meta;
}
