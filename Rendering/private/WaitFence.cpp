#include "WaitFence.h"

#include "DXFence.h"

rendering::WaitFence::WaitFence(DXFence& fence) :
	m_fence(fence)
{
}

bool rendering::WaitFence::Wait(UINT64 signal, std::string& errorMessage)
{
    HANDLE h = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (h == nullptr)
    {
        errorMessage = "Can't create Fence Event!";
        return false;
    }

    HRESULT hRes = m_fence.GetFence()->SetEventOnCompletion(signal, h);
    if (FAILED(hRes))
    {
        errorMessage = "Can't Add Event to Fence!";
        return false;
    }

    WaitForSingleObject(h, INFINITE);
    return true;
}