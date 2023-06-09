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

rendering::ILightsManager* rendering::deferred::GetLightsManager()
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

rendering::DXTexture* rendering::deferred::GetGBufferDiffuseTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXGBufferDuffuseTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Diffuse tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::deferred::GetGBufferSpecularTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXGBufferSpecularTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Specular tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::deferred::GetGBufferNormalTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXGBufferNormalTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Normal tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}

rendering::DXTexture* rendering::deferred::GetGBufferPositionTex()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXGBufferPositionTexMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find GBuffer Position tex!";
	}

	DXTexture* tex = static_cast<DXTexture*>(obj);
	return tex;
}
