#pragma once

#include "DXScene.h"
#include "SceneSettings.h"
#include "AnimRepo.h"

namespace animation
{
	rendering::DXScene* GetScene();
	collada::SceneSettings* GetSceneSettings();
	animation::AnimRepo* GetAnimRepo();
}