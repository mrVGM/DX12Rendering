#pragma once

#include <d3d12.h>
#include <string>

namespace rendering
{
	class DXFence;

	class WaitFence
	{
	private:
		DXFence& m_fence;
	public:
		WaitFence(DXFence& fence);
		void Wait(UINT64 signal);
	};
}