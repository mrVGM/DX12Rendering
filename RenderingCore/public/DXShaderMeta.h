#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXShaderMeta : public BaseObjectMeta
	{
	public:
		static const DXShaderMeta& GetInstance();
		DXShaderMeta();
	};
}