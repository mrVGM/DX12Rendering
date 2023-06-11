#pragma once

#include "LightsManager.h"
#include "ICamera.h"
#include "DXScene.h"
#include "DXDeferredRP.h"
#include "DXMutableBuffer.h"

namespace rendering::deferred
{
	LightsManager* GetLightsManager();
	ICamera* GetCamera();
	DXScene* GetScene();
	DXMutableBuffer* GetCameraBuffer();
	DXDeferredRP* GetdeferredRP();
}