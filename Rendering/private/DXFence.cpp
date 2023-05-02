#include "DXFence.h"

#include "BaseObjectContainer.h"
#include "DXDevice.h"
#include "DXDeviceMeta.h"

#include <iostream>

bool rendering::DXFence::Create(std::string& errorMessage)
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	DXDevice* device = nullptr;
	{
		BaseObject* obj = container.GetObjectOfClass(DXDeviceMeta::GetInstance());
		if (!obj)
		{
			errorMessage = "No Device found!";
			return false;
		}

		device = static_cast<DXDevice*>(obj);
	}

	HRESULT hr = device->GetDevice().CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(hr))
	{
		errorMessage = "Can't Create Fence!";
		return false;
	}
	return true;
}

ID3D12Fence* rendering::DXFence::GetFence() const
{
	return m_fence.Get();
}

rendering::DXFence::DXFence(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
	std::string error;
	bool res = Create(error);
	if (!res)
	{
		std::cerr << error << std::endl;
	}
}

rendering::DXFence::~DXFence()
{
}