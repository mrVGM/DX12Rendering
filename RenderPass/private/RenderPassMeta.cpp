#include "RenderPassMeta.h"

namespace
{
	rendering::RenderPassMeta m_meta;
}

rendering::RenderPassMeta::RenderPassMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::RenderPassMeta& rendering::RenderPassMeta::GetInstance()
{
	return m_meta;
}
