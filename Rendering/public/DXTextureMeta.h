#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXTextureMeta : public BaseObjectMeta
	{
	public:
		static const DXTextureMeta& GetInstance();
		DXTextureMeta();
	};
}