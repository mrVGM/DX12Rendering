#include "RenderingCore.h"

#include "JobSystem.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "DXSwapChain.h"

void rendering::core::Boot()
{
	new jobs::JobSystem(MainJobSystemMeta::GetInstance(), 1);
	new jobs::JobSystem(LoadJobSystemMeta::GetInstance(), 5);

	new rendering::Window();
	new DXDevice();
	new DXCommandQueue();
	new DXCopyCommandQueue();
	new DXSwapChain();


}