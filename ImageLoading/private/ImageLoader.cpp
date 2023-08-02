#include "ImageLoader.h"

#include "ImageLoaderMeta.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::image_loading::ImageLoader::ImageLoader() :
	BaseObject(ImageLoaderMeta::GetInstance())
{
	THROW_ERROR(
		CoInitialize(nullptr),
		"CoInitialize Error!"
	)

	THROW_ERROR(
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<LPVOID*>(m_factory.GetAddressOf())
		),
		"Can't Create WIC Imaging Factory!"
	)

	bool t = true;
}

rendering::image_loading::ImageLoader::~ImageLoader()
{
}

#undef THROW_ERROR