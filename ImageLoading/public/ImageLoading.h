#pragma once

#include "BaseObject.h"

#include <string>

namespace rendering
{
	class DXTexture;
}

namespace rendering::image_loading
{
	void Boot();
	DXTexture* GetImage(const std::string& image);
}