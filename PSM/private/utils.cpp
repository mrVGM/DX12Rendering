#include "utils.h"

#include "ILightsManagerMeta.h"
#include "PSM.h"
#include "ICameraMeta.h"
#include "DXSceneMeta.h"
#include "DXCameraBufferMeta.h"

#include "DXGBufferDuffuseTexMeta.h"
#include "DXGBufferSpecularTexMeta.h"
#include "DXGBufferNormalTexMeta.h"
#include "DXGBufferPositionTexMeta.h"
#include "DXRenderTextureVertexBufferMeta.h"
#include "PSMMeta.h"

#include "DXBuffer.h"
#include "DXMaterialRepoMeta.h"

#include "BaseObjectContainer.h"

rendering::ILightsManager* rendering::psm::GetLightsManager()
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

rendering::psm::PSM* rendering::psm::GetPSM()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(PSMMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find PSM!";
	}

	PSM* cascadedSM = static_cast<PSM*>(obj);
	return cascadedSM;
}


rendering::ICamera* rendering::psm::GetCamera()
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

rendering::DXScene* rendering::psm::GetScene()
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

rendering::DXMutableBuffer* rendering::psm::GetCameraBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXCameraBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Camera Buffer!";
	}

	DXMutableBuffer* buffer = static_cast<DXMutableBuffer*>(obj);
	return buffer;
}

rendering::DXTexture* rendering::psm::GetGBufferDiffuseTex()
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

rendering::DXTexture* rendering::psm::GetGBufferSpecularTex()
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

rendering::DXTexture* rendering::psm::GetGBufferNormalTex()
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

rendering::DXTexture* rendering::psm::GetGBufferPositionTex()
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

rendering::DXBuffer* rendering::psm::GetRenderTextureBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXRenderTextureVertexBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Render Texture Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}

rendering::DXMaterialRepo* rendering::psm::GetMaterialRepo()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXMaterialRepoMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Material Repo!";
	}

	DXMaterialRepo* materialRepo = static_cast<DXMaterialRepo*>(obj);
	return materialRepo;
}
