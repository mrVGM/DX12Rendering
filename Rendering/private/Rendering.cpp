#include "Rendering.h"

#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXSwapChain.h"

#include <iostream>

void rendering::InitBaseObjects()
{
	new DXDevice();
	new DXCommandQueue();
	new DXSwapChain();

	std::cout << "Base Rendering Objects created!" << std::endl;
}