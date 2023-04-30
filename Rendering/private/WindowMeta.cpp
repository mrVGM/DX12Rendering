#include "WindowMeta.h"

namespace
{
	rendering::WindowMeta m_windowMeta;
}

rendering::WindowMeta::WindowMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::WindowMeta& rendering::WindowMeta::GetInstance()
{
	return m_windowMeta;
}
