#include "ILightsManagerMeta.h"

namespace
{
	rendering::ILightsManagerMeta m_meta;
}

rendering::ILightsManagerMeta::ILightsManagerMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::ILightsManagerMeta& rendering::ILightsManagerMeta::GetInstance()
{
	return m_meta;
}
