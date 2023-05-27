#pragma once

#include "LightsManager.h"
#include "cascadedSM/CascadedSM.h"
#include "ICamera.h"
#include "DXScene.h"

namespace rendering::deferred
{
	LightsManager* GetLightsManager();
	CascadedSM* GetCascadedSM();
	ICamera* GetCamera();
	DXScene* GetScene();
}