#pragma once

#include "LightsManager.h"
#include "cascadedSM/CascadedSM.h"
#include "ICamera.h"
#include "DXScene.h"
#include "DXCameraBufferMeta.h"
#include "DXMutableBuffer.h"

namespace rendering::deferred
{
	LightsManager* GetLightsManager();
	CascadedSM* GetCascadedSM();
	ICamera* GetCamera();
	DXScene* GetScene();
	DXMutableBuffer* GetCameraBuffer();
}