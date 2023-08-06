#include "utils.h"

#include "Resources/QuadVertexBufferMeta.h"
#include "Resources/QuadIndexBufferMeta.h"
#include "Resources/QuadInstanceBufferMeta.h"

#include "DXMaterialRepoMeta.h"
#include "DXOverlayRPMeta.h"
#include "FontAssetMeta.h"

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

rendering::DXMutableBuffer* rendering::overlay::GetQuadInstanceBuffer()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(QuadInstanceBufferMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Quad Index Buffer!";
	}

	DXMutableBuffer* buffer = static_cast<DXMutableBuffer*>(obj);
	return buffer;
}

rendering::overlay::DXOverlayRP* rendering::overlay::GetOverlayRP()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(DXOverlayRPMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Overlay RP!";
	}

	DXOverlayRP* overlayRP = static_cast<DXOverlayRP*>(obj);
	return overlayRP;
}

rendering::DXMaterialRepo* rendering::overlay::GetMaterialRepo()
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

rendering::image_loading::FontAsset* rendering::overlay::GetFontAsset()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(image_loading::FontAssetMeta::GetInstance());

	if (!obj)
	{
		return nullptr;
	}

	image_loading::FontAsset* fontAsset = static_cast<image_loading::FontAsset*>(obj);
	return fontAsset;
}
