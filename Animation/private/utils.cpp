#include "utils.h"

#include "DXSceneMeta.h"
#include "SceneSettingsMeta.h"
#include "AnimRepoMeta.h"

#include "BaseObjectContainer.h"

rendering::DXScene* animation::GetScene()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(rendering::DXSceneMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Scene!";
	}

	rendering::DXScene* scene = static_cast<rendering::DXScene*>(obj);
	return scene;
}

collada::SceneSettings* animation::GetSceneSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(collada::SceneSettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Scene Settings!";
	}

	collada::SceneSettings* sceneSettings = static_cast<collada::SceneSettings*>(obj);
	return sceneSettings;
}

animation::AnimRepo* animation::GetAnimRepo()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(animation::AnimRepoMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Anim Repo!";
	}

	animation::AnimRepo* animRepo = static_cast<animation::AnimRepo*>(obj);
	return animRepo;
}