#include "utils.h"

#include "ILightsManagerMeta.h"
#include "CascadedSMMeta.h"
#include "ICameraMeta.h"
#include "DXSceneMeta.h"
#include "DXCameraBufferMeta.h"

#include "DXGBufferDuffuseTexMeta.h"
#include "DXGBufferSpecularTexMeta.h"
#include "DXGBufferNormalTexMeta.h"
#include "DXGBufferPositionTexMeta.h"

#include "DXBuffer.h"

#include "BaseObjectContainer.h"

rendering::ILightsManager* rendering::cascaded::GetLightsManager()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(ILightsManagerMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Lights Manager!";
	}

	ILightsManager* lightsManager = static_cast<ILightsManager*>(obj);
	return lightsManager;
}

rendering::CascadedSM* rendering::cascaded::GetCascadedSM()
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


rendering::ICamera* rendering::cascaded::GetCamera()
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

rendering::DXScene* rendering::cascaded::GetScene()
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

rendering::DXBuffer* rendering::cascaded::GetCameraBuffer()
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

rendering::DXTexture* rendering::cascaded::GetGBufferDiffuseTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferDuffuseTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Diffuse tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::cascaded::GetGBufferSpecularTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferSpecularTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Specular tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::cascaded::GetGBufferNormalTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferNormalTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Normal tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::cascaded::GetGBufferPositionTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferPositionTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Position tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}
