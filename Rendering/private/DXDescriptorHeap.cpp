#include "DXDescriptorHeap.h"

#include "BaseObjectMeta.h"

#include "DXDepthStencilDescriptorHeapMeta.h"

#include "RenderUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::DXDescriptorHeap::DXDescriptorHeap(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
}

rendering::DXDescriptorHeap::~DXDescriptorHeap()
{
}


rendering::DXDescriptorHeap* rendering::DXDescriptorHeap::CreateDSVDescriptorHeap(rendering::DXTexture& depthStencilTex)
{
	DXDevice* device = utils::GetDevice();
	DXDescriptorHeap* res = new DXDescriptorHeap(DXDepthStencilDescriptorHeapMeta::GetInstance());

	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		THROW_ERROR(
			device->GetDevice().CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&res->m_descriptorHeap)),
			"Can't Create DSV Heap!"
		);
	}

	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(res->m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

		device->GetDevice().CreateDepthStencilView(depthStencilTex.GetTexture(), &depthStencilDesc, dsvHandle);
	}

	return res;
}


ID3D12DescriptorHeap* rendering::DXDescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap.Get();
}

#undef THROW_ERROR