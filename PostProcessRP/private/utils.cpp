#include "utils.h"

#include "DXCameraBufferMeta.h"

#include "Resources/CanvasVertexBufferMeta.h"
#include "Resources/CanvasIndexBufferMeta.h"

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
