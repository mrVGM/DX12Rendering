#include "ShadowMapMeta.h"

namespace
{
	rendering::ShadowMapMeta m_meta;
}

rendering::ShadowMapMeta::ShadowMapMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::ShadowMapMeta& rendering::ShadowMapMeta::GetInstance()
{
	return m_meta;
}
