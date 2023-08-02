#include "ImageLoader.h"

#include "ImageLoaderMeta.h"

rendering::image_loading::ImageLoader::ImageLoader() :
	BaseObject(ImageLoaderMeta::GetInstance())
{
}

rendering::image_loading::ImageLoader::~ImageLoader()
{
}