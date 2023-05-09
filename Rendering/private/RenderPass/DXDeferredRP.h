#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXDeferredRP : public RenderPass
	{
	public:
		enum GBuffTextureType
		{
			Diffuse = 0,
			Normal = 1,
			Position = 2
		};
	private:
		ID3D12CommandList** m_commandListsCache = nullptr;
		int m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		UINT m_rtvDescriptorSize = -1;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endList;

		void Create();
		void RenderUnlit();
	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		void Prepare() override;
		void Execute() override;

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleFor(GBuffTextureType texType);
	};
}