#include "DXOutlineMaterialMeta.h"

#include "DXMaterialMeta.h"

namespace
{
	rendering::DXOutlineMaterialMeta m_meta;
}

rendering::DXOutlineMaterialMeta::DXOutlineMaterialMeta() :
	BaseObjectMeta(&DXMaterialMeta::GetInstance())
{
}

const rendering::DXOutlineMaterialMeta& rendering::DXOutlineMaterialMeta::GetInstance()
{
	return m_meta;
}
