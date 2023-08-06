#include "FontAssetMeta.h"

namespace
{
	rendering::image_loading::FontAssetMeta m_meta;
}

rendering::image_loading::FontAssetMeta::FontAssetMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::image_loading::FontAssetMeta& rendering::image_loading::FontAssetMeta::GetInstance()
{
	return m_meta;
}
