#include "ImageLoading.h"

#include "ImageLoader.h"
#include "ImageLoadingSettings.h"

#include "DXTexture.h"
#include "Job.h"


namespace
{
	rendering::image_loading::ImageLoader* m_imageLoader = nullptr;
	rendering::image_loading::ImageLoadingSettings* m_imageLoadingSettings = nullptr;
}

void rendering::image_loading::Boot()
{
	 m_imageLoadingSettings = new ImageLoadingSettings();
	 m_imageLoader = new ImageLoader();

	 m_imageLoader->StartLoadingImages();
}

rendering::DXTexture* rendering::image_loading::GetImage(const std::string& image)
{
	auto it = m_imageLoadingSettings->GetSettings().m_images.find(image);
	if (it == m_imageLoadingSettings->GetSettings().m_images.end())
	{
		return nullptr;
	}

	return m_imageLoader->GetImage(it->second.m_imageFile);
}

const std::string* rendering::image_loading::GetImageDescriptionFile(const std::string& image)
{
	auto it = m_imageLoadingSettings->GetSettings().m_images.find(image);
	if (it == m_imageLoadingSettings->GetSettings().m_images.end())
	{
		return nullptr;
	}

	return &it->second.m_descriptionFile;
}