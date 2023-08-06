#pragma once

#include "DXBuffer.h"
#include "DXMutableBuffer.h"
#include "DXOverlayRP.h"
#include "DXMaterialRepo.h"
#include "FontAsset.h"

namespace rendering::overlay
{
	struct QuadVertex
	{
		float m_position[3];
		float m_uv[2];
	};

	struct CharInfo
	{
		float m_position[4];
		float m_uvPos[4];
		float m_color[4];
	};

	DXBuffer* GetQuadVertexBuffer();
	DXBuffer* GetQuadIndexBuffer();
	DXMutableBuffer* GetQuadInstanceBuffer();
	DXOverlayRP* GetOverlayRP();
	DXMaterialRepo* GetMaterialRepo();
	image_loading::FontAsset* GetFontAsset();
}