#pragma once

#include "DXScene.h"
#include "SceneSettings.h"

namespace animation
{
	rendering::DXScene* GetScene();
	collada::SceneSettings* GetSceneSettings();
}