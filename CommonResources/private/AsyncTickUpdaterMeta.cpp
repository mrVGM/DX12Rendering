#include "AsyncTickUpdaterMeta.h"

namespace
{
	rendering::AsyncTickUpdaterMeta m_meta;
}

rendering::AsyncTickUpdaterMeta::AsyncTickUpdaterMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::AsyncTickUpdaterMeta& rendering::AsyncTickUpdaterMeta::GetInstance()
{
	return m_meta;
}
