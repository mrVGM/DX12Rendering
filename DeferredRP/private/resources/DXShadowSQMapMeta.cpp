#include "DXShadowSQMapMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXShadowSQMapMeta m_instance;
}

rendering::DXShadowSQMapMeta::DXShadowSQMapMeta() :
	BaseObjectMeta(&rendering::DXTextureMeta::GetInstance())
{
}

const rendering::DXShadowSQMapMeta& rendering::DXShadowSQMapMeta::GetInstance()
{
	return m_instance;
}