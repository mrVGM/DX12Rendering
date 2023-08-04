#include "ImageLoading.h"

#include "ImageLoader.h"

#include "DXTexture.h"
#include "Job.h"


namespace
{
	rendering::image_loading::ImageLoader* m_loader = nullptr;
}

void rendering::image_loading::Boot()
{
	m_loader = new ImageLoader();

	class Done : public jobs::Job
	{
	public:
		void Do() override
		{
			bool t = true;
		}
	};

	m_loader->LoadImageFromFile("ConsolasFont.png", new Done());
}

rendering::DXTexture* rendering::image_loading::GetImage(const std::string& image)
{
	return m_loader->GetImage(image);
}