#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <wrl.h>
#include <wincodec.h>
#include <string>
#include <map>

namespace rendering
{
	class DXTexture;
}

namespace rendering::image_loading
{
	class ImageLoader : public BaseObject
	{
		Microsoft::WRL::ComPtr<IWICImagingFactory> m_factory;
		std::map<std::string, DXTexture*> m_imagesRepo;

		void LoadImageFromFile(const std::string& imageFile, jobs::Job* done);
	public:
		ImageLoader();
		virtual ~ImageLoader();

		DXTexture* GetImage(const std::string& image);

		void StartLoadingImages();
	};
}