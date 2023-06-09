#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXRenderTextureVertexBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXRenderTextureVertexBufferMeta& GetInstance();
		DXRenderTextureVertexBufferMeta();
	};
}