#include "RenderDataUpdaterMeta.h"

namespace
{
	rendering::RenderDataUpdaterMeta m_meta;
}

rendering::RenderDataUpdaterMeta::RenderDataUpdaterMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::RenderDataUpdaterMeta& rendering::RenderDataUpdaterMeta::GetInstance()
{
	return m_meta;
}
