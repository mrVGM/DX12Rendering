#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "DXBuffer.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering::unlit
{
	DXBuffer* GetCameraBuffer();
}