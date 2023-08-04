#include "ImageTextureMeta.h"

#include "DXTextureMeta.h"

namespace
{
	rendering::image_loading::ImageTextureMeta m_meta;
}

rendering::image_loading::ImageTextureMeta::ImageTextureMeta() :
	BaseObjectMeta(&DXTextureMeta::GetInstance())
{
}

const rendering::image_loading::ImageTextureMeta& rendering::image_loading::ImageTextureMeta::GetInstance()
{
	return m_meta;
}
