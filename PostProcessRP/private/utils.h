#pragma once

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

namespace rendering
{
	DXMutableBuffer* GetCameraBuffer();
	DXBuffer* GetCanvasVertexBuffer();
	DXBuffer* GetCanvasIndexBuffer();
}