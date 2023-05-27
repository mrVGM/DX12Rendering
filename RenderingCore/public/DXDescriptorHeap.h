#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include <list>

class BaseObjectMeta;

namespace rendering
{
	class DXTexture;

	class DXDescriptorHeap : public BaseObject
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		UINT m_descriptorSize = 0;
	public:
		DXDescriptorHeap(const BaseObjectMeta& meta);
		virtual ~DXDescriptorHeap();

		static DXDescriptorHeap* CreateDSVDescriptorHeap(const BaseObjectMeta& meta, const std::list<DXTexture*>& textures);
		static DXDescriptorHeap* CreateRTVDescriptorHeap(const BaseObjectMeta& meta, const std::list<DXTexture*>& textures);
		static DXDescriptorHeap* CreateSRVDescriptorHeap(const BaseObjectMeta& meta, const std::list<DXTexture*>& textures);

		ID3D12DescriptorHeap* GetDescriptorHeap();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(UINT index);
	};
}