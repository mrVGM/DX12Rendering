#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXPixelShaderMeta : public BaseObjectMeta
	{
	public:
		static const DXPixelShaderMeta& GetInstance();
		DXPixelShaderMeta();
	};
}