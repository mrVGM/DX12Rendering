#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>

#include "d3dx12.h"

namespace rendering
{
	class DXDevice : public BaseObject
	{
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
        Microsoft::WRL::ComPtr<ID3D12Device> m_device;

        void Create();

    public:
        DXDevice();
        virtual ~DXDevice();

        ID3D12Device& GetDevice();
        IDXGIFactory4* GetFactory() const;
	};

}