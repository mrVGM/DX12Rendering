#pragma once

#include "LightsManager.h"
#include "ICamera.h"
#include "DXScene.h"
#include "DXCameraBufferMeta.h"
#include "DXDeferredRP.h"

namespace rendering::deferred
{
	LightsManager* GetLightsManager();
	ICamera* GetCamera();
	DXScene* GetScene();
	DXBuffer* GetCameraBuffer();
	DXDeferredRP* GetdeferredRP();
}