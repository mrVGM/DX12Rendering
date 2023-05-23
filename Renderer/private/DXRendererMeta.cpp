#include "DXRendererMeta.h"

namespace
{
	rendering::DXRendererMeta m_meta;
}

rendering::DXRendererMeta::DXRendererMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXRendererMeta& rendering::DXRendererMeta::GetInstance()
{
	return m_meta;
}
