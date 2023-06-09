#include "utils.h"

#include "BaseObjectContainer.h"

#include "MainJobSystemMeta.h"

jobs::JobSystem* notifications::GetMainJobSystem()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(rendering::MainJobSystemMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Main Job System!";
	}

	jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);
	return jobSystem;
}
