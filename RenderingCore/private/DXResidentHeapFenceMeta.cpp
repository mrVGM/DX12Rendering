#include "DXResidentHeapFenceMeta.h"

#include "DXFenceMeta.h"

namespace
{
	rendering::DXResidentHeapFenceMeta m_meta;
}

rendering::DXResidentHeapFenceMeta::DXResidentHeapFenceMeta() :
	BaseObjectMeta(&DXFenceMeta::GetInstance())
{
}

const rendering::DXResidentHeapFenceMeta& rendering::DXResidentHeapFenceMeta::GetInstance()
{
	return m_meta;
}
