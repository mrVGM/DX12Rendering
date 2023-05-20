#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXVertexShaderMeta : public BaseObjectMeta
	{
	public:
		static const DXVertexShaderMeta& GetInstance();
		DXVertexShaderMeta();
	};
}