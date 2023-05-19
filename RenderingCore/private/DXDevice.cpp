#include "dxDevice.h"

#include "DXDeviceMeta.h"

#include <iostream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


void rendering::DXDevice::Create()
{
    using Microsoft::WRL::ComPtr;

    UINT dxgiFactoryFlags = 0;

#if DEBUG
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    THROW_ERROR(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)), "Can't create DXGIFactoty!")

    {
        THROW_ERROR(D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ), "Can't Create device");
    }
}

#undef THROW_ERROR

ID3D12Device& rendering::DXDevice::GetDevice()
{
    return *m_device.Get();
}

IDXGIFactory4* rendering::DXDevice::GetFactory() const
{
    return m_factory.Get();
}

rendering::DXDevice::DXDevice() :
    BaseObject(DXDeviceMeta::GetInstance())
{
    Create();
}

rendering::DXDevice::~DXDevice()
{
}