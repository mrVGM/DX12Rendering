#include "utils.h"

#include "LightsManagerMeta.h"

#include "BaseObjectContainer.h"

rendering::LightsManager* rendering::deferred::GetLightsManager()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(LightsManagerMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Lights Manager!";
	}

	LightsManager* lightsManager = static_cast<LightsManager*>(obj);
	return lightsManager;
}
