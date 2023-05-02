#pragma once

#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "Window.h"

namespace rendering
{
	namespace utils
	{
		rendering::Window* GetWindow();
		rendering::DXDevice* GetDevice();
		rendering::DXSwapChain* GetSwapChain();
		rendering::DXCommandQueue* GetCommandQueue();
	}
}