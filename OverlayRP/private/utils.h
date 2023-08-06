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
		int m_charID;
	};

	DXBuffer* GetQuadVertexBuffer();
	DXBuffer* GetQuadIndexBuffer();
	DXMutableBuffer* GetQuadInstanceBuffer();
	DXOverlayRP* GetOverlayRP();
	DXMaterialRepo* GetMaterialRepo();
	image_loading::FontAsset* GetFontAsset();
}