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
#include "DXSceneMeta.h"
#include "UpdaterMeta.h"
#include "DXRendererMeta.h"
#include "DXDepthStencilTextureMeta.h"
#include "DXDepthStencilDescriptorHeapMeta.h"

#include "DXShader.h"
#include "DXVertexShaderMeta.h"
#include "DXPixelShaderMeta.h"

#include "Materials/DXUnlitErrorMaterial.h"
#include "Materials/DXUnlitErrorMaterialMeta.h"

#include "DXMaterialRepo.h"
#include "DXMaterialRepoMeta.h"

#include "Lights/LightsManager.h"
#include "Lights/LightsManagerMeta.h"

namespace
{
	rendering::Updater* m_updater = nullptr;
	rendering::Window* m_window = nullptr;
	rendering::DXRenderer* m_renderer = nullptr;
	rendering::DXDevice* m_device = nullptr;
	rendering::DXSwapChain* m_swapChain = nullptr;
	rendering::DXCommandQueue* m_commandQueue = nullptr;
	rendering::DXCopyCommandQueue* m_copyCommandQueue = nullptr;
	rendering::DXCamera* m_camera = nullptr;
	rendering::DXBuffer* m_cameraBuffer = nullptr;
	jobs::JobSystem* m_mainJobSystem = nullptr;
	jobs::JobSystem* m_loadJobSystem = nullptr;

	rendering::DXScene* m_scene = nullptr;

	rendering::DXMaterialRepo* m_materialRepo = nullptr;

	rendering::LightsManager* m_lightsManager = nullptr;


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

rendering::Updater* rendering::utils::GetUpdater()
{
	if (m_updater)
	{
		return m_updater;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(UpdaterMeta::GetInstance());
	m_updater = static_cast<Updater*>(obj);
	return m_updater;
}

rendering::DXRenderer* rendering::utils::GetRenderer()
{
	if (m_renderer)
	{
		return m_renderer;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXRendererMeta::GetInstance());
	m_renderer = static_cast<DXRenderer*>(obj);
	return m_renderer;
}

rendering::DXMaterialRepo* rendering::utils::GetMaterialRepo()
{
	if (m_materialRepo)
	{
		return m_materialRepo;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(DXMaterialRepoMeta::GetInstance());
	m_materialRepo = static_cast<DXMaterialRepo*>(obj);
	return m_materialRepo;
}

rendering::LightsManager* rendering::utils::GetLightsManager()
{
	if (m_lightsManager)
	{
		return m_lightsManager;
	}
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	BaseObject* obj = container.GetObjectOfClass(LightsManagerMeta::GetInstance());
	m_lightsManager = static_cast<LightsManager*>(obj);
	return m_lightsManager;
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
	GetRenderer();

	return;
}
