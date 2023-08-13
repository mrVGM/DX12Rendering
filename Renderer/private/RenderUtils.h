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
#include "DXMutableBuffer.h"

#include "AppSettings.h"

#include "RendererSettings.h"

#include "DXUnlitErrorMaterial.h"

#include "DXDescriptorHeap.h"

#include "JobSystem.h"

namespace rendering
{
	namespace utils
	{
		void CacheObjects();

		Updater* GetUpdater();
		Window* GetWindow();
		DXRenderer* GetRenderer();
		DXDevice* GetDevice();
		DXSwapChain* GetSwapChain();
		DXCommandQueue* GetCommandQueue();
		DXCopyCommandQueue* GetCopyCommandQueue();
		DXCamera* GetCamera();
		DXMutableBuffer* GetCameraBuffer();
		DXScene* GetScene();
		settings::AppSettings* GetAppSettings();
		RendererSettings* GetRendererSettings();

		rendering::DXMaterialRepo* GetMaterialRepo();

		void RunSync(jobs::Job* job);
		void RunAsync(jobs::Job* job);
		void DisposeBaseObject(BaseObject& baseObject);
	}
}