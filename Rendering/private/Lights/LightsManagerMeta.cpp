#include "Lights/LightsManagerMeta.h"

namespace
{
	rendering::LightsManagerMeta m_instance;
}

rendering::LightsManagerMeta::LightsManagerMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::LightsManagerMeta& rendering::LightsManagerMeta::GetInstance()
{
	return m_instance;
}