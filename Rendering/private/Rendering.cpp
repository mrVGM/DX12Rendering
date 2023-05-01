#include "Rendering.h"

#include "DXDevice.h"
#include "DXCommandQueue.h"

void rendering::InitBaseObjects()
{
	new DXDevice();
	new DXCommandQueue();

}