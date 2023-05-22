#pragma once

#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "Window.h"
#include "DXCamera.h"
#include "DXBuffer.h"
#include "DXScene.h"
#include "DXShader.h"
#include "Updater.h"
#include "DXRenderer.h"
#include "DXMaterialRepo.h"
#include "DXTexture.h"
#include "Lights/LightsManager.h"

#include "Materials/DXUnlitErrorMaterial.h"

#include "DXDescriptorHeap.h"

#include "JobSystem.h"

namespace rendering
{
	namespace utils
	{
		void CacheObjects();

		rendering::Updater* GetUpdater();
		rendering::Window* GetWindow();
		rendering::DXRenderer* GetRenderer();
		rendering::DXDevice* GetDevice();
		rendering::DXSwapChain* GetSwapChain();
		rendering::DXCommandQueue* GetCommandQueue();
		rendering::DXCopyCommandQueue* GetCopyCommandQueue();
		rendering::DXCamera* GetCamera();
		rendering::DXBuffer* GetCameraBuffer();
		rendering::DXScene* GetScene();

		rendering::DXMaterialRepo* GetMaterialRepo();

		void RunSync(jobs::Job* job);
		void RunAsync(jobs::Job* job);
		void DisposeBaseObject(BaseObject& baseObject);
	}
}