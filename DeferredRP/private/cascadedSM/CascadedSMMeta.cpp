#include "CascadedSMMeta.h"

namespace
{
	rendering::CascadedSMMeta m_meta;
}

rendering::CascadedSMMeta::CascadedSMMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::CascadedSMMeta& rendering::CascadedSMMeta::GetInstance()
{
	return m_meta;
}
