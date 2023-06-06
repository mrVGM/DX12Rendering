#include "utils.h"

#include "LightsManagerMeta.h"
#include "cascadedSM/CascadedSMMeta.h"
#include "ICameraMeta.h"
#include "DXSceneMeta.h"
#include "DXCameraBufferMeta.h"

#include "DXBuffer.h"

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

rendering::CascadedSM* rendering::deferred::GetCascadedSM()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(CascadedSMMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Cascaded SM!";
	}

	CascadedSM* cascadedSM = static_cast<CascadedSM*>(obj);
	return cascadedSM;
}


rendering::ICamera* rendering::deferred::GetCamera()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(ICameraMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Camera!";
	}

	ICamera* camera = static_cast<ICamera*>(obj);
	return camera;
}

rendering::DXScene* rendering::deferred::GetScene()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXSceneMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Camera!";
	}

	DXScene* scene = static_cast<DXScene*>(obj);
	return scene;
}

rendering::DXBuffer* rendering::deferred::GetCameraBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXCameraBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Camera Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}
