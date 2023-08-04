#pragma once

#include "BaseObjectMeta.h"

namespace rendering::image_loading
{
	class ImageTextureMeta : public BaseObjectMeta
	{
	public:
		static const ImageTextureMeta& GetInstance();
		ImageTextureMeta();
	};
}