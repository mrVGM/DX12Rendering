#include "DXDescriptorHeap.h"

#include "BaseObjectMeta.h"

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


rendering::DXDescriptorHeap* rendering::DXDescriptorHeap::CreateDSVDescriptorHeap(const BaseObjectMeta& meta, rendering::DXTexture& depthStencilTex)
{
	DXDevice* device = utils::GetDevice();
	DXDescriptorHeap* res = new DXDescriptorHeap(meta);

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

rendering::DXDescriptorHeap* rendering::DXDescriptorHeap::CreateRTVDescriptorHeap(const BaseObjectMeta& meta, const std::list<DXTexture*>& textures)
{
	using Microsoft::WRL::ComPtr;

	DXDevice* device = utils::GetDevice();

	DXDescriptorHeap* res = new DXDescriptorHeap(meta);

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = textures.size();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		THROW_ERROR(
			device->GetDevice().CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&res->m_descriptorHeap)),
			"Can't create a descriptor heap!")

		res->m_descriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(res->m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (auto it = textures.begin(); it != textures.end(); ++it)
		{
			device->GetDevice().CreateRenderTargetView((*it)->GetTexture(), nullptr, rtvHandle);
			rtvHandle.Offset(1, res->m_descriptorSize);
		}
	}

    return res;
}

rendering::DXDescriptorHeap* rendering::DXDescriptorHeap::CreateSRVDescriptorHeap(const BaseObjectMeta& meta, const std::list<DXTexture*>& textures)
{
	using Microsoft::WRL::ComPtr;

    DXDevice* device = utils::GetDevice();

    DXDescriptorHeap* res = new DXDescriptorHeap(meta);

    {
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = textures.size();
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        THROW_ERROR(
            device->GetDevice().CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&res->m_descriptorHeap)),
            "Can't create a descriptor heap!")

        res->m_descriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(res->m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

        for (auto it = textures.begin(); it != textures.end(); ++it)
        {
            DXTexture* tex = *it;
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = tex->GetTextureDescription().Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture3D.MipLevels = 1;
            srvDesc.Texture2D.MipLevels = 1;

            device->GetDevice().CreateShaderResourceView(tex->GetTexture(), &srvDesc, srvHandle);
            srvHandle.Offset(1, res->m_descriptorSize);
        }
    }

	return res;
}


D3D12_CPU_DESCRIPTOR_HANDLE rendering::DXDescriptorHeap::GetDescriptorHandle(UINT index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < index; ++i)
	{
		handle.Offset(m_descriptorSize);
	}

	return handle;
}


ID3D12DescriptorHeap* rendering::DXDescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap.Get();
}

#undef THROW_ERROR