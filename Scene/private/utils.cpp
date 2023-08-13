#include "utils.h"

#include "BaseObjectContainer.h"

#include "SceneSettingsMeta.h"

collada::SceneSettings* rendering::GetSceneSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(collada::SceneSettingsMeta::GetInstance());
	if (!obj)
	{
		throw "Can't Scene Settings!";
	}

	collada::SceneSettings* sceneSettings = static_cast<collada::SceneSettings*>(obj);
	return sceneSettings;
}