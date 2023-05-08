#include "DXDepthStencilTextureMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::DXDepthStencilTextureMeta m_meta;
}

rendering::DXDepthStencilTextureMeta::DXDepthStencilTextureMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::DXDepthStencilTextureMeta& rendering::DXDepthStencilTextureMeta::GetInstance()
{
	return m_meta;
}
