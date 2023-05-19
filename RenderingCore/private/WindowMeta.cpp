#include "WindowMeta.h"

namespace
{
	rendering::WindowMeta m_meta;
}

rendering::WindowMeta::WindowMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::WindowMeta& rendering::WindowMeta::GetInstance()
{
	return m_meta;
}
