#include "WaitFence.h"

#include "DXFence.h"

rendering::WaitFence::WaitFence(DXFence& fence) :
	m_fence(fence)
{
}

void rendering::WaitFence::Wait(UINT64 signal)
{
    HANDLE h = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (h == nullptr)
    {
        throw "Can't create Fence Event!";
    }

    HRESULT hRes = m_fence.GetFence()->SetEventOnCompletion(signal, h);
    if (FAILED(hRes))
    {
        throw "Can't Add Event to Fence!";
    }

    DWORD res = WaitForSingleObject(h, INFINITE);
}