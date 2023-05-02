#include "RenderFenceMeta.h"

#include "DXFenceMeta.h"

namespace
{
	rendering::RenderFenceMeta m_instance;
}

rendering::RenderFenceMeta::RenderFenceMeta() :
	BaseObjectMeta(DXFenceMeta::GetInstance())
{
}

const rendering::RenderFenceMeta& rendering::RenderFenceMeta::GetInstance()
{
	return m_instance;
}