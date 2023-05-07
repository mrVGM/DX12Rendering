#include "DXMaterialRepoMeta.h"

namespace
{
	rendering::DXMaterialRepoMeta m_meta;
}

rendering::DXMaterialRepoMeta::DXMaterialRepoMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXMaterialRepoMeta& rendering::DXMaterialRepoMeta::GetInstance()
{
	return m_meta;
}
