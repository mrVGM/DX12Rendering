#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXComputeShaderMeta : public BaseObjectMeta
	{
	public:
		static const DXComputeShaderMeta& GetInstance();
		DXComputeShaderMeta();
	};
}