#include "TickUpdaterMeta.h"

namespace
{
	rendering::TickUpdaterMeta m_meta;
}

rendering::TickUpdaterMeta::TickUpdaterMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::TickUpdaterMeta& rendering::TickUpdaterMeta::GetInstance()
{
	return m_meta;
}
