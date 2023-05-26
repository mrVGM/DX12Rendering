#include "DXLightsCalculationsMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXLightsCalculationsMaterialMeta m_meta;
}

rendering::DXLightsCalculationsMaterialMeta::DXLightsCalculationsMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXLightsCalculationsMaterialMeta& rendering::DXLightsCalculationsMaterialMeta::GetInstance()
{
	return m_meta;
}
