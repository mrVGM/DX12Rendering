#include "utils.h"

#include "Resources/QuadVertexBufferMeta.h"
#include "Resources/QuadIndexBufferMeta.h"

#include "BaseObjectContainer.h"

rendering::DXBuffer* rendering::overlay::GetQuadVertexBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(QuadVertexBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Quad Vertex Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}

rendering::DXBuffer* rendering::overlay::GetQuadIndexBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(QuadIndexBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Quad Index Buffer!";
	}

	DXBuffer* buffer = static_cast<DXBuffer*>(obj);
	return buffer;
}
