#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXDepthStencilTextureMeta : public BaseObjectMeta
	{
	public:
		static const DXDepthStencilTextureMeta& GetInstance();
		DXDepthStencilTextureMeta();
	};
}