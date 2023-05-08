#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXTexture;

	class DXDescriptorHeap : public BaseObject
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	public:
		DXDescriptorHeap(const BaseObjectMeta& meta);
		virtual ~DXDescriptorHeap();

		static DXDescriptorHeap* CreateDSVDescriptorHeap(DXTexture& depthStencilTex);
		ID3D12DescriptorHeap* GetDescriptorHeap();
	};
}