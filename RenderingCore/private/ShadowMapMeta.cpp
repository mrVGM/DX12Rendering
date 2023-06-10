#include "ShadowMapMeta.h"

namespace
{
	rendering::shadow_mapping::ShadowMapMeta m_meta;
}

rendering::shadow_mapping::ShadowMapMeta::ShadowMapMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::shadow_mapping::ShadowMapMeta& rendering::shadow_mapping::ShadowMapMeta::GetInstance()
{
	return m_meta;
}
