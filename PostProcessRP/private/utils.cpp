#include "utils.h"

#include "DXCameraBufferMeta.h"

#include "Resources/CanvasVertexBufferMeta.h"
#include "Resources/CanvasIndexBufferMeta.h"

#include "DXCameraDepthTexMeta.h"
#include "DXGBufferNormalTexMeta.h"
#include "DXGBufferPositionTexMeta.h"

#include "BaseObjectContainer.h"

rendering::DXMutableBuffer* rendering::GetCameraBuffer()
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

rendering::DXBuffer* rendering::GetCanvasVertexBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(CanvasVertexBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Canvas Vertex Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}

rendering::DXBuffer* rendering::GetCanvasIndexBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(CanvasIndexBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Canvas Index Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}

rendering::DXTexture* rendering::GetCameraDepthTetxure()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXCameraDepthTexMeta::GetInstance());

	if (!obj)
	{
		return nullptr;
	}

	DXTexture* texture = static_cast<DXTexture*>(obj);
	return texture;
}

rendering::DXTexture* rendering::GetNormalsTetxure()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferNormalTexMeta::GetInstance());

	if (!obj)
	{
		return nullptr;
	}

	DXTexture* texture = static_cast<DXTexture*>(obj);
	return texture;
}

rendering::DXTexture* rendering::GetPositionTetxure()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(deferred::DXGBufferPositionTexMeta::GetInstance());

	if (!obj)
	{
		return nullptr;
	}

	DXTexture* texture = static_cast<DXTexture*>(obj);
	return texture;
}
