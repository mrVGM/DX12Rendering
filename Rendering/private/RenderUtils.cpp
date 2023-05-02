#pragma once

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"
#include "DXSwapChainMeta.h"
#include "DXCommandQueueMeta.h"

rendering::Window* rendering::utils::GetWindow()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(WindowMeta::GetInstance());
	return static_cast<rendering::Window*>(obj);
}

rendering::DXDevice* rendering::utils::GetDevice()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXDeviceMeta::GetInstance());
	return static_cast<rendering::DXDevice*>(obj);
}

rendering::DXSwapChain* rendering::utils::GetSwapChain()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXSwapChainMeta::GetInstance());
	return static_cast<rendering::DXSwapChain*>(obj);
}

rendering::DXCommandQueue* rendering::utils::GetCommandQueue()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCommandQueueMeta::GetInstance());
	return static_cast<rendering::DXCommandQueue*>(obj);
}
