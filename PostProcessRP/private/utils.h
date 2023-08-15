#pragma once

#include "DXBuffer.h"
#include "DXMutableBuffer.h"
#include "DXTexture.h"

namespace rendering
{
	DXMutableBuffer* GetCameraBuffer();
	DXBuffer* GetCanvasVertexBuffer();
	DXBuffer* GetCanvasIndexBuffer();

	DXTexture* GetCameraDepthTetxure();
	DXTexture* GetNormalsTetxure();
	DXTexture* GetPositionTetxure();
}