#include "DXPostLightsCalculationsMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXPostLightsCalculationsMaterialMeta m_meta;
}

rendering::DXPostLightsCalculationsMaterialMeta::DXPostLightsCalculationsMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXPostLightsCalculationsMaterialMeta& rendering::DXPostLightsCalculationsMaterialMeta::GetInstance()
{
	return m_meta;
}
