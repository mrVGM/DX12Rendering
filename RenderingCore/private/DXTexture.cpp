#include "DXTexture.h"

#include "d3dx12.h"

#include "DXDepthStencilTextureMeta.h"
#include "CoreUtils.h"

#include "DXHeap.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	rendering::DXDevice* m_device = nullptr;

	void CacheObjects()
	{
		if (!m_device)
		{
			m_device = rendering::core::utils::GetDevice();
		}
	}
}

rendering::DXTexture::DXTexture(const BaseObjectMeta& meta, const D3D12_RESOURCE_DESC& description) :
	BaseObject(meta),
	m_description(description)
{
	CacheObjects();
}

rendering::DXTexture::~DXTexture()
{
}

void rendering::DXTexture::Place(DXHeap& heap, UINT64 heapOffset)
{
	D3D12_RESOURCE_DESC textureDesc = GetTextureDescription();
	bool isDS = textureDesc.Flags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	THROW_ERROR(
		m_device->GetDevice().CreatePlacedResource(
			heap.GetHeap(),
			heapOffset,
			&textureDesc,
			isDS ? D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
			isDS ? &depthOptimizedClearValue : nullptr,
			IID_PPV_ARGS(&m_texture)),
		"Can't place texture in the heap!")

	m_heap = &heap;
}

ID3D12Resource* rendering::DXTexture::GetTexture() const
{
	return m_texture.Get();
}

const D3D12_RESOURCE_DESC& rendering::DXTexture::GetTextureDescription() const
{
	return m_description;
}

rendering::DXTexture* rendering::DXTexture::CreateDepthStencilTexture(const BaseObjectMeta& meta, UINT width, UINT height)
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;

	CD3DX12_RESOURCE_DESC textureDesc = {};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1,
		0,
		1,
		0,
		flags);

	DXTexture* res = new DXTexture(meta, textureDesc);
	return res;
}

D3D12_RESOURCE_ALLOCATION_INFO rendering::DXTexture::GetTextureAllocationInfo()
{
	D3D12_RESOURCE_DESC textureDesc = GetTextureDescription();
	D3D12_RESOURCE_ALLOCATION_INFO info = m_device->GetDevice().GetResourceAllocationInfo(0, 1, &textureDesc);
	return info;
}

rendering::DXTexture* rendering::DXTexture::CreateRenderTargetTexture(const BaseObjectMeta& meta, UINT width, UINT height)
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	CD3DX12_RESOURCE_DESC textureDesc = {};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1,
		0,
		1,
		0,
		flags);

	DXTexture* res = new DXTexture(meta, textureDesc);
	return res;
}

#undef THROW_ERROR