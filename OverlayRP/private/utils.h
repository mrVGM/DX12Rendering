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

	struct CharInfo
	{
		float m_position[4];
		int m_charID;
	};

	DXBuffer* GetQuadVertexBuffer();
	DXBuffer* GetQuadIndexBuffer();
	DXMutableBuffer* GetQuadInstanceBuffer();
}