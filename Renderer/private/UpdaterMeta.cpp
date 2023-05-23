#include "UpdaterMeta.h"

namespace
{
	rendering::UpdaterMeta m_meta;
}

rendering::UpdaterMeta::UpdaterMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::UpdaterMeta& rendering::UpdaterMeta::GetInstance()
{
	return m_meta;
}
