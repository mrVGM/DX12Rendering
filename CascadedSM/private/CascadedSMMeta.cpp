#include "CascadedSMMeta.h"

#include "ShadowMapMeta.h"

namespace
{
	rendering::CascadedSMMeta m_meta;
}

rendering::CascadedSMMeta::CascadedSMMeta() :
	BaseObjectMeta(&shadow_mapping::ShadowMapMeta::GetInstance())
{
}

const rendering::CascadedSMMeta& rendering::CascadedSMMeta::GetInstance()
{
	return m_meta;
}
