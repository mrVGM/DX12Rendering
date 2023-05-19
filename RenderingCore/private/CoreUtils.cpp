#include "CoreUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"
#include "DXCommandQueueMeta.h"

namespace rendering::utils
{
	Window* GetWindow()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj =  container.GetObjectOfClass(WindowMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Window!";
		}

		Window* wnd = static_cast<Window*>(obj);
		return wnd;
	}

	DXDevice* GetDevice()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXDeviceMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Device!";
		}

		DXDevice* device = static_cast<DXDevice*>(obj);
		return device;
	}

	DXCommandQueue* GetCommandQueue()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXCommandQueueMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find CommandQueue!";
		}

		DXCommandQueue* commandQueue = static_cast<DXCommandQueue*>(obj);
		return commandQueue;
	}
}