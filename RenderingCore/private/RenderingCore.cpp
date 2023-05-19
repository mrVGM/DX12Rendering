#include "RenderingCore.h"

#include "JobSystem.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"
#include "ResidentHeapJobSystemMeta.h"

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "DXSwapChain.h"
#include "DXFence.h"
#include "ResourceUtils/DXCopyBuffers.h"

#include "DXResidentHeapFenceMeta.h"

#include "CoreUtils.h"

void rendering::core::Boot()
{
	new jobs::JobSystem(MainJobSystemMeta::GetInstance(), 1);
	new jobs::JobSystem(LoadJobSystemMeta::GetInstance(), 5);
	new jobs::JobSystem(ResidentHeapJobSystemMeta::GetInstance(), 1);

	new rendering::Window();
	new DXDevice();
	new DXCommandQueue();
	new DXCopyCommandQueue();
	new DXSwapChain();
	new DXFence(DXResidentHeapFenceMeta::GetInstance());
	new DXCopyBuffers();

	utils::CacheJobSystems();
}