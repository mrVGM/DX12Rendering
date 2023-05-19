#pragma once

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"

namespace rendering::utils
{
	Window* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
}