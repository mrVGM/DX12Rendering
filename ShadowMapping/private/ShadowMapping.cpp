#include "ShadowMapping.h"

#include "CascadedSM.h"

namespace
{
	rendering::shadow_mapping::ShadowMap* m_shadowMap = nullptr;
}

rendering::shadow_mapping::ShadowMap* rendering::shadow_mapping::GetShadowMap()
{
	if (m_shadowMap)
	{
		return m_shadowMap;
	}

	m_shadowMap = new CascadedSM();
	return m_shadowMap;
}