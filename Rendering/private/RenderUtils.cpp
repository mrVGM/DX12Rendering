#pragma once

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"
#include "DXSwapChainMeta.h"
#include "DXCommandQueueMeta.h"
#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"
#include "DXCameraMeta.h"
#include "DXCameraBufferMeta.h"

namespace
{
	rendering::Window* m_window = nullptr;
	rendering::DXDevice* m_device = nullptr;
	rendering::DXSwapChain* m_swapChain = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;
	rendering::DXCamera* m_camera = nullptr;
	rendering::DXBuffer* m_cameraBuffer = nullptr;
	jobs::JobSystem* m_mainJobSystem = nullptr;
	jobs::JobSystem* m_loadJobSystem = nullptr;
}

rendering::Window* rendering::utils::GetWindow()
{
	if (m_window)
	{
		return m_window;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(WindowMeta::GetInstance());
	m_window = static_cast<rendering::Window*>(obj);
	return m_window;
}

rendering::DXDevice* rendering::utils::GetDevice()
{
	if (m_device)
	{
		return m_device;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXDeviceMeta::GetInstance());
	m_device = static_cast<rendering::DXDevice*>(obj);
	return m_device;
}

rendering::DXSwapChain* rendering::utils::GetSwapChain()
{
	if (m_swapChain)
	{
		return m_swapChain;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXSwapChainMeta::GetInstance());
	m_swapChain = static_cast<rendering::DXSwapChain*>(obj);
	return m_swapChain;
}

rendering::DXCommandQueue* rendering::utils::GetCommandQueue()
{
	if (m_commandQueue)
	{
		return m_commandQueue;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCommandQueueMeta::GetInstance());
	m_commandQueue = static_cast<rendering::DXCommandQueue*>(obj);
	return m_commandQueue;
}

jobs::JobSystem* rendering::utils::GetMainJobSystem()
{
	if (m_mainJobSystem)
	{
		return m_mainJobSystem;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(MainJobSystemMeta::GetInstance());
	m_mainJobSystem = static_cast<jobs::JobSystem*>(obj);
	return m_mainJobSystem;
}

jobs::JobSystem* rendering::utils::GetLoadJobSystem()
{
	if (m_loadJobSystem)
	{
		return m_loadJobSystem;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(LoadJobSystemMeta::GetInstance());
	m_loadJobSystem = static_cast<jobs::JobSystem*>(obj);
	return m_loadJobSystem;
}

rendering::DXCamera* rendering::utils::GetCamera()
{
	if (m_camera)
	{
		return m_camera;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCameraMeta::GetInstance());
	m_camera = static_cast<DXCamera*>(obj);
	return m_camera;
}

rendering::DXBuffer* rendering::utils::GetCameraBuffer()
{
	if (m_cameraBuffer)
	{
		return m_cameraBuffer;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCameraBufferMeta::GetInstance());
	m_cameraBuffer = static_cast<DXBuffer*>(obj);
	return m_cameraBuffer;
}

void rendering::utils::CacheObjects()
{
	GetWindow();
	GetDevice();
	GetSwapChain();
	GetCommandQueue();
	GetMainJobSystem();
	GetLoadJobSystem();
	GetCamera();
	GetCameraBuffer();
}
