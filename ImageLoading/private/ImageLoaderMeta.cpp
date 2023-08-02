#include "ImageLoaderMeta.h"

namespace
{
	rendering::image_loading::ImageLoaderMeta m_meta;
}

rendering::image_loading::ImageLoaderMeta::ImageLoaderMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::image_loading::ImageLoaderMeta& rendering::image_loading::ImageLoaderMeta::GetInstance()
{
	return m_meta;
}
