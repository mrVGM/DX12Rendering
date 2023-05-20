#pragma once

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "DXSwapChain.h"

#include "Job.h"

namespace rendering::core::utils
{
	Window* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
	DXCopyCommandQueue* GetCopyCommandQueue();
	DXSwapChain* GetSwapChain();

	void CacheJobSystems();
	void RunSync(jobs::Job* job);
	void RunAsync(jobs::Job* job);
	void DisposeBaseObject(BaseObject& baseObject);
}