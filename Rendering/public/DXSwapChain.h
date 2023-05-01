#pragma once

#include "BaseObject.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>


namespace rendering
{
	class DXSwapChain : public BaseObject
	{
		static const UINT FrameCount = 2;
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		UINT m_frameIndex;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		UINT m_rtvDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

		bool Create(std::string& errorMessage);

		
	public:
		DXSwapChain();

		bool Present(std::string& errorMessage);
		void UpdateCurrentFrameIndex();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVDescriptor() const;
		ID3D12Resource* GetCurrentRenderTarget() const;
	};
}