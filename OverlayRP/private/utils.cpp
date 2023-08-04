#include "utils.h"

#include "Resources/QuadVertexBufferMeta.h"

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
