#include "ResourceUtils/DXCopyFenceMeta.h"

namespace
{
	rendering::DXCopyFenceMeta m_meta;
}

rendering::DXCopyFenceMeta::DXCopyFenceMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCopyFenceMeta& rendering::DXCopyFenceMeta::GetInstance()
{
	return m_meta;
}
