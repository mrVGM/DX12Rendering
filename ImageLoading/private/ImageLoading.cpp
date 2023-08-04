#include "ImageLoading.h"

#include "ImageLoader.h"

#include "Job.h"

void rendering::image_loading::Boot()
{
	ImageLoader* loader = new ImageLoader();

	class Done : public jobs::Job
	{
	public:
		void Do() override
		{
			bool t = true;
		}
	};

	loader->LoadImageFromFile("ConsolasFont.png", new Done());
}