#include "utils.h"

#include "BaseObject.h"
#include "BaseObjectContainer.h"

#include "ImageLoaderMeta.h"
#include "ImageLoadingSettingsMeta.h"

rendering::image_loading::ImageLoader* rendering::image_loading::GetImageLoader()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(ImageLoaderMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Image Loader!";
	}

	ImageLoader* imageLoader = static_cast<ImageLoader*>(obj);
	return imageLoader;
}

rendering::image_loading::ImageLoadingSettings* rendering::image_loading::GetImageLoadingSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(ImageLoadingSettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Image Loading Settings!";
	}

	ImageLoadingSettings* imageLoadingSettings = static_cast<ImageLoadingSettings*>(obj);
	return imageLoadingSettings;
}
