#include "DataDefReaderMeta.h"

#include "SettingsReaderMeta.h"

namespace
{
	reflection::DataDefReaderMeta m_meta;
}

reflection::DataDefReaderMeta::DataDefReaderMeta() :
	BaseObjectMeta(&settings::SettingsReaderMeta::GetInstance())
{
}

const reflection::DataDefReaderMeta& reflection::DataDefReaderMeta::GetInstance()
{
	return m_meta;
}
