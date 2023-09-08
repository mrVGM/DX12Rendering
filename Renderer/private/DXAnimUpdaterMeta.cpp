#include "DXAnimUpdaterMeta.h"

#include "TickUpdaterMeta.h"

namespace
{
	rendering::DXAnimUpdaterMeta m_meta;
}

rendering::DXAnimUpdaterMeta::DXAnimUpdaterMeta() :
	BaseObjectMeta(&TickUpdaterMeta::GetInstance())
{
}

const rendering::DXAnimUpdaterMeta& rendering::DXAnimUpdaterMeta::GetInstance()
{
	return m_meta;
}
