#include "LightsManagerMeta.h"

#include "ILightsManagerMeta.h"

namespace
{
	rendering::LightsManagerMeta m_instance;
}

rendering::LightsManagerMeta::LightsManagerMeta() :
	BaseObjectMeta(&ILightsManagerMeta::GetInstance())
{
}

const rendering::LightsManagerMeta& rendering::LightsManagerMeta::GetInstance()
{
	return m_instance;
}