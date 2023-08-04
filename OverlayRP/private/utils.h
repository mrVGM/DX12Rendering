#pragma once

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

namespace rendering::overlay
{
	struct QuadVertex
	{
		float m_position[4];
		float m_uv[2];
	};

	DXBuffer* GetQuadVertexBuffer();
}