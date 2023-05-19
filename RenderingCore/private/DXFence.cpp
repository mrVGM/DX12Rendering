#include "DXFence.h"

#include "CoreUtils.h"

#include <iostream>

void rendering::DXFence::Create()
{
	DXDevice* device = core::utils::GetDevice();
	if (!device)
	{
		throw "No Device found!";
	}

	HRESULT hr = device->GetDevice().CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(hr))
	{
		throw "Can't Create Fence!";
	}
}

ID3D12Fence* rendering::DXFence::GetFence() const
{
	return m_fence.Get();
}

rendering::DXFence::DXFence(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
	Create();
}

rendering::DXFence::~DXFence()
{
}