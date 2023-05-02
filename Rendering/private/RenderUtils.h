#pragma once

#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "Window.h"
#include "DXCamera.h"
#include "DXBuffer.h"

#include "JobSystem.h"

namespace rendering
{
	namespace utils
	{
		void CacheObjects();
		rendering::Window* GetWindow();
		rendering::DXDevice* GetDevice();
		rendering::DXSwapChain* GetSwapChain();
		rendering::DXCommandQueue* GetCommandQueue();
		rendering::DXCamera* GetCamera();
		rendering::DXBuffer* GetCameraBuffer();
		jobs::JobSystem* GetMainJobSystem();
		jobs::JobSystem* GetLoadJobSystem();
	}
}