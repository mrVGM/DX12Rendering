#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXHeap;

	class DXTexture : public BaseObject
	{
	private:
		DXHeap* m_heap = nullptr;

		D3D12_RESOURCE_DESC m_description;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;

	public:
		DXTexture(const BaseObjectMeta& meta, const D3D12_RESOURCE_DESC& description);
		virtual ~DXTexture();

		void Place(ID3D12Heap* heap, UINT64 heapOffset);

		const D3D12_RESOURCE_DESC& GetTextureDescription() const;
		D3D12_RESOURCE_ALLOCATION_INFO GetTextureAllocationInfo();
		ID3D12Resource* GetTexture() const;

		static DXTexture* CreateDepthStencilTexture(UINT width, UINT height);
		static DXTexture* CreateRenderTargetTexture(const BaseObjectMeta& meta, UINT width, UINT height);
	};
}