#include "DXTextureMeta.h"

namespace
{
	rendering::DXTextureMeta m_meta;
}

rendering::DXTextureMeta::DXTextureMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXTextureMeta& rendering::DXTextureMeta::GetInstance()
{
	return m_meta;
}
