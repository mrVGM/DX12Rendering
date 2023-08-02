#pragma once

#include "BaseObject.h"

#include <wrl.h>
#include <wincodec.h>

namespace rendering::image_loading
{
	class ImageLoader : public BaseObject
	{
		Microsoft::WRL::ComPtr<IWICImagingFactory> m_factory;

	public:
		ImageLoader();
		virtual ~ImageLoader();
	};
}