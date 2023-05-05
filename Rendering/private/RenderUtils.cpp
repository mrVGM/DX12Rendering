#pragma once

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"
#include "DXSwapChainMeta.h"
#include "DXCommandQueueMeta.h"
#include "DXCopyCommandQueueMeta.h"
#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"
#include "DXCameraMeta.h"
#include "DXCameraBufferMeta.h"
#include "ResourceUtils/DXCopyBuffersMeta.h"
#include "DXSceneMeta.h"

#include "DXShader.h"
#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"

#include "Materials/DXUnlitMaterial.h"
#include "Materials/DXUnlitMaterialMeta.h"

namespace
{
	rendering::Window* m_window = nullptr;
	rendering::DXDevice* m_device = nullptr;
	rendering::DXSwapChain* m_swapChain = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;
	rendering::DXCopyCommandQueue* m_copyCommandQueue = nullptr;
	rendering::DXCamera* m_camera = nullptr;
	rendering::DXBuffer* m_cameraBuffer = nullptr;
	jobs::JobSystem* m_mainJobSystem = nullptr;
	jobs::JobSystem* m_loadJobSystem = nullptr;

	rendering::DXScene* m_scene = nullptr;
	rendering::DXCopyBuffers* m_copyBuffers = nullptr;

	rendering::DXShader* m_unlitPixelShader = nullptr;
	rendering::DXShader* m_unlitVertexShader = nullptr;

	rendering::DXUnlitMaterial* m_unlitMaterial = nullptr;


	jobs::JobSystem* GetMainJobSystem()
	{
		if (m_mainJobSystem)
		{
			return m_mainJobSystem;
		}

		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::MainJobSystemMeta::GetInstance());
		m_mainJobSystem = static_cast<jobs::JobSystem*>(obj);
		return m_mainJobSystem;
	}

	jobs::JobSystem* GetLoadJobSystem()
	{
		if (m_loadJobSystem)
		{
			return m_loadJobSystem;
		}

		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::LoadJobSystemMeta::GetInstance());
		m_loadJobSystem = static_cast<jobs::JobSystem*>(obj);
		return m_loadJobSystem;
	}
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

rendering::DXCopyCommandQueue* rendering::utils::GetCopyCommandQueue()
{
	if (m_copyCommandQueue)
	{
		return m_copyCommandQueue;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCopyCommandQueueMeta::GetInstance());
	m_copyCommandQueue = static_cast<rendering::DXCopyCommandQueue*>(obj);
	return m_copyCommandQueue;
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

rendering::DXScene* rendering::utils::GetScene()
{
	if (m_scene)
	{
		return m_scene;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXSceneMeta::GetInstance());
	m_scene = static_cast<DXScene*>(obj);
	return m_scene;
}

rendering::DXCopyBuffers* rendering::utils::GetCopyBuffers()
{
	if (m_copyBuffers)
	{
		return m_copyBuffers;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXCopyBuffersMeta::GetInstance());
	m_copyBuffers = static_cast<DXCopyBuffers*>(obj);
	return m_copyBuffers;
}

rendering::DXShader* rendering::utils::GetUnlitVertexShader()
{
	if (m_unlitVertexShader)
	{
		return m_unlitVertexShader;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXVertexShaderMeta::GetInstance());
	m_unlitVertexShader = static_cast<DXShader*>(obj);
	return m_unlitVertexShader;
}

rendering::DXShader* rendering::utils::GetUnlitPixelShader()
{
	if (m_unlitPixelShader)
	{
		return m_unlitPixelShader;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXPixelShaderMeta::GetInstance());
	m_unlitPixelShader = static_cast<DXShader*>(obj);
	return m_unlitPixelShader;
}

rendering::DXUnlitMaterial* rendering::utils::GetUnlitMaterial()
{
	if (m_unlitMaterial)
	{
		return m_unlitMaterial;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXUnlitMaterialMeta::GetInstance());
	m_unlitMaterial = static_cast<DXUnlitMaterial*>(obj);
	return m_unlitMaterial;
}

void rendering::utils::RunSync(jobs::Job* job)
{
	GetMainJobSystem()->ScheduleJob(job);
}

void rendering::utils::RunAsync(jobs::Job* job)
{
	GetLoadJobSystem()->ScheduleJob(job);
}

void rendering::utils::DisposeBaseObject(BaseObject& baseObject)
{
	class Dispose : public jobs::Job
	{
	private:
		BaseObject& m_object;
	public:
		Dispose(BaseObject& object) :
			m_object(object)
		{
		}

		void Do() override
		{
			delete &m_object;
		}
	};

	utils::RunSync(new Dispose(baseObject));
}

void rendering::utils::CacheObjects()
{
	GetWindow();
	GetDevice();
	GetSwapChain();
	GetCommandQueue();
	GetCopyCommandQueue();
	GetMainJobSystem();
	GetLoadJobSystem();
	GetCamera();
	GetCameraBuffer();
	GetUnlitVertexShader();
	GetUnlitPixelShader();
	GetUnlitMaterial();

	GetScene();
}
