#include "AppEntryPointMeta.h"

namespace
{
	settings::AppEntryPointMeta m_meta;
}

settings::AppEntryPointMeta::AppEntryPointMeta() :
	BaseObjectMeta(nullptr)
{
}

const settings::AppEntryPointMeta& settings::AppEntryPointMeta::GetInstance()
{
	return m_meta;
}
