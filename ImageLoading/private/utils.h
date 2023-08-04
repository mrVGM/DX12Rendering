#pragma once

#include "ImageLoader.h"
#include "ImageLoadingSettings.h"

namespace rendering::image_loading
{
	ImageLoader* GetImageLoader();
	ImageLoadingSettings* GetImageLoadingSettings();
}