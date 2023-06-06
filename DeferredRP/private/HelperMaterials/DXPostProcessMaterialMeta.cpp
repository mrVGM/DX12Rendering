#include "DXPostProcessMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXPostProcessMaterialMeta m_meta;
}

rendering::DXPostProcessMaterialMeta::DXPostProcessMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXPostProcessMaterialMeta& rendering::DXPostProcessMaterialMeta::GetInstance()
{
	return m_meta;
}
